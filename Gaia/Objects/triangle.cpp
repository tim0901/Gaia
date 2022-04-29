#include "Triangle.h"


#ifndef __APPLE__

#pragma warning( push, 0 )
#define TINYOBJLOADER_IMPLEMENTATION
#include "../Dependencies/tiny_obj_loader.h"
#pragma warning( pop )

#else

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#define TINYOBJLOADER_IMPLEMENTATION
#include "../Dependencies/tiny_obj_loader.h"
#pragma clang diagnostic pop

#endif

static std::string GetBaseDir(const std::string& filepath) {
	if (filepath.find_last_of("/\\") != std::string::npos)
		return filepath.substr(0, filepath.find_last_of("/\\"));
	return "";
}

// Load a mesh from a file
bool Mesh::LoadMesh(const std::string filename, const Vec3d &position, std::vector<Material*> mats) {

	// IT'S TINYOBJLOADER TIME! YAAAAAAAAY!

	std::string baseDirectory = GetBaseDir(filename);

	// Containers to put things in
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errors, warn;

	// Lets load the obj file
	bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &errors, filename.c_str(), baseDirectory.c_str(), true);

	// Any warnings?
	if (!warn.empty())
		std::cerr << "Warning: " << warn;
	// Any errors?
	if (!errors.empty())
		std::cerr << "Error: " << errors;

	if (!success)
		return 0;

	// Resize the containers of the mesh class to store things in
	vertexPositions = std::make_unique<Vec3f[]>(attrib.vertices.size());
	normalPositions = std::make_unique<Vec3f[]>(attrib.normals.size());
	uvs = std::make_unique<Vec2f[]>(attrib.texcoords.size());

	for (const auto& shape : shapes) {

		for (const auto& index : shape.mesh.indices) {
			// Load vertices
			int vertexIndex = 3 * index.vertex_index;
			vertexPositions[index.vertex_index] = Vec3f(attrib.vertices[vertexIndex], attrib.vertices[vertexIndex + 1], attrib.vertices[vertexIndex + 2]) * scale + position;
			vertexIndices.push_back(index.vertex_index);

			// Load normals if available
			if (attrib.normals.size() > 0) {
				int normalIndex = 3 * index.normal_index;
				normalPositions[index.normal_index] = Vec3f(attrib.normals[normalIndex], attrib.normals[normalIndex + 1], attrib.normals[normalIndex + 2]);
				normalIndices.push_back(index.normal_index);
			}

			// Load UVs if available
			// TODO: FIX THIS - 99% SURE THIS IS WRONG
			if (attrib.texcoords.size() > 0) {
				// We have UVs to load
				uvs[index.texcoord_index] = Vec2f(attrib.texcoords[2 * index.texcoord_index], attrib.texcoords[2 * index.texcoord_index + 1]);
				uvIndices.push_back(index.texcoord_index);
			}
		}	

		// Load materials
		for (int i = 0; i < shape.mesh.material_ids.size(); i++) {
			materialIndexes.push_back(shape.mesh.material_ids[i]);
		}
	}

	nTris = vertexIndices.size() / 3;
	nVerts = attrib.vertices.size();
	nUVs = attrib.texcoords.size();

	subdivisionLevel = 0;

	if (subdivisionLevel > 0) {
		
		std::cout << "Subdividing mesh... ";

		using namespace OpenSubdiv;

		Sdc::SchemeType type = Sdc::SCHEME_LOOP; // Use loop subdivision

		Sdc::Options options;
		options.SetVtxBoundaryInterpolation(Sdc::Options::VTX_BOUNDARY_EDGE_ONLY); // Set interpolation options

		// We know that all faces are triangles as our obj files are triangulated by tiny obj loader
		int* numVertsPerFace = new int[nTris];
		for (int i = 0; i < nTris; i++) {
			numVertsPerFace[i] = 3;
		}

		// Load the vertex indices vector into an array so we can pass it to the topology descriptor
		int* vertexIndicesArray = new int[vertexIndices.size()]{ 0 };
		for (int i = 0; i < vertexIndices.size(); i++) {
			vertexIndicesArray[i] = vertexIndices[i];
		}

		// Populate a TopologyDescriptor with raw vertex data
		Far::TopologyDescriptor descriptor;
		descriptor.numVertices = nVerts;
		descriptor.numFaces = nTris;
		descriptor.numVertsPerFace = numVertsPerFace;
		descriptor.vertIndicesPerFace = vertexIndicesArray;
	
		int* uvIndicesArray = new int[uvIndices.size()]{ 0 };
		for (int i = 0; i < uvIndices.size(); i++) {
			uvIndicesArray[i] = uvIndices[i];
		}

		// Create a face-varying channel descriptor (for UVs)
		Far::TopologyDescriptor::FVarChannel channels[1];
		channels[0].numValues = attrib.texcoords.size();
		channels[0].valueIndices = uvIndicesArray;

		// Add the FVarChannel to the TopologyDescriptor
		descriptor.numFVarChannels = 1;
		descriptor.fvarChannels = channels;

		// Instantiate a TopologyRefiner from our TopologyDescriptor
		Far::TopologyRefiner* refiner = Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Create(descriptor, Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Options(type, options));

		// Refine topology to max level
		Far::TopologyRefiner::UniformOptions refineOptions(subdivisionLevel);
		refineOptions.fullTopologyInLastLevel = true;
		refiner->RefineUniform(refineOptions);

		// Move old vertices and UVs into a container that OpenSubDiv can use
		std::vector<Vec3f> vBuffer(refiner->GetNumVerticesTotal());
		Vec3f* vertBuffer = &vBuffer[0];
		for (int i = 0; i < nVerts; i++) {
			vertBuffer[i] = vertexPositions[i];
		}

		std::vector<Vec2f> fvBufferUV(refiner->GetNumFVarValuesTotal(0));
		Vec2f* uvBuffer = &fvBufferUV[0];
		for (int i = 0; i < uvIndices.size(); i++) {
			uvBuffer[i] = uvs[i];
		}

		// Fetch pointers to our raw data
		Vec3f* srcVerts = &vertBuffer[0];
		Vec2f* srcUVs = &uvBuffer[0];

		std::vector<int> tempMaterialList;

		Far::PrimvarRefiner primvarRefiner(*refiner);
		
		for (int level = 1; level <= subdivisionLevel; level++) {
			Vec3f* dstVert = srcVerts + refiner->GetLevel(level - 1).GetNumVertices();
			Vec2f* dstFVarUV = srcUVs + refiner->GetLevel(level - 1).GetNumFVarValues(0);

			primvarRefiner.Interpolate(level, srcVerts, dstVert);
			primvarRefiner.Interpolate(level, srcUVs, dstFVarUV);

			srcVerts = dstVert;
			srcUVs = dstFVarUV;

			// Use the map from old faces to new faces to retain correct material usage
			Far::TopologyLevel const& refCurrentLevel = refiner->GetLevel(level);

			nTris = refCurrentLevel.GetNumFaces();

			for (int i = 0; i < nTris; i++) {
				tempMaterialList.push_back(materialIndexes.at(refCurrentLevel.GetFaceParentFace(i)));
			}
			// Swap the vectors over
			materialIndexes.swap(tempMaterialList);
			tempMaterialList.clear();
		}

		// Calculate normals
		Far::TopologyLevel const& refLastLevel = refiner->GetLevel(subdivisionLevel);
		nVerts = refLastLevel.GetNumVertices();
		nTris = refLastLevel.GetNumFaces();
		nUVs = refLastLevel.GetNumFVarValues(0);
		int firstOfLastVerts = refiner->GetNumVerticesTotal() - nVerts;
		int firstOfLastUVs = refiner->GetNumFVarValuesTotal(0) - nUVs;

		// Delete previous vertices, normals and UVs and prepare their arrays for the new ones
		vertexIndices.clear();
		normalIndices.clear();
		uvIndices.clear();
		vertexPositions.reset();
		normalPositions.reset();
		uvs.reset();
		vertexPositions = std::make_unique<Vec3f[]>(nVerts);
		normalPositions = std::make_unique<Vec3f[]>(nVerts);
		uvs = std::make_unique<Vec2f[]>(nUVs);

		// For each face (tri) in the mesh
		for (int f = 0; f < nTris; f++) {

			// Fetch the vertices and UVs for this face
			Far::ConstIndexArray faceVertices = refLastLevel.GetFaceVertices(f);
			Far::ConstIndexArray fUVs = refLastLevel.GetFaceFVarValues(f, 0);

			// Calculate the normal vector from the vertices
			const Vec3f v0 = vertBuffer[firstOfLastVerts + faceVertices[0]];
			const Vec3f v1 = vertBuffer[firstOfLastVerts + faceVertices[1]];
			const Vec3f v2 = vertBuffer[firstOfLastVerts + faceVertices[2]];

			Vec3f normal = unit_vector(cross_product(v1 - v0, v2 - v0));

			for (int vInFace = 0; vInFace < faceVertices.size(); vInFace++) {
				// Add the calculated normal to each vertex of the face
				int vertIndex = faceVertices[vInFace];
				normalPositions[vertIndex] += normal;
				// Recreate normal incices table
				normalIndices.push_back(vertIndex);

				// Store vertices and their indices
				vertexPositions[vertIndex] = vertBuffer[firstOfLastVerts + faceVertices[vInFace]];
				vertexIndices.push_back(vertIndex);
			}

			// Store UVs and their indices
			for (int uvInFace = 0; uvInFace < fUVs.size(); uvInFace++) {
				int uvIndex = fUVs[uvInFace];
				uvs[uvIndex] = uvBuffer[firstOfLastUVs + fUVs[uvInFace]];
				uvIndices.push_back(uvIndex);
			}

			// Store triangle
			if (materialIndexes[f] == -1) {
				// No material information for this triangle
				tris->push_back(std::make_shared<Triangle>(this, mats[0], f));
			}
			else if (mats.size() < materialIndexes[f]) {
				// Not enough materials have been supplied
				std::cout << "Missing materials for mesh\n";
				tris->push_back(std::make_shared<Triangle>(this, mats[0], f));
			}
			else {
				tris->push_back(std::make_shared<Triangle>(this, mats[materialIndexes[f]], f));
			}
		}


		// Cleanup
		delete[] numVertsPerFace;
		delete[] vertexIndicesArray;
		delete[] uvIndicesArray;
		vBuffer.clear();
		fvBufferUV.clear();

		std::cout << "Complete\n";
		
	}
	else {
		// Store the loaded triangles
		for (size_t i = 0; i < nTris; i++) {

			if (materialIndexes[i] == -1) {
				// No material information for this triangle
				tris->push_back(std::make_shared<Triangle>(this, mats[0], i));
			}
			else if (mats.size() < materialIndexes[i]) {
				// Not enough materials have been supplied
				std::cout << "Missing materials for mesh\n";
				tris->push_back(std::make_shared<Triangle>(this, mats[0], i));
			}
			else {
				tris->push_back(std::make_shared<Triangle>(this, mats[materialIndexes[i]], i));
			}
		}
	}

	std::cout << "Triangles: " << nTris << "\n";
	std::cout << "Vertices: " << nVerts << "\n";
	std::cout << "VertexIndices: " << vertexIndices.size() << "\n";
	std::cout << "Normals: " << nVerts << "\n";
	std::cout << "NormalIndices: " << normalIndices.size() << "\n";

	// Cleanup!
	shapes.clear();
	materials.clear();

	return true;
}

// Perform ray-triangle intersection test
bool Triangle::Intersect(Ray r, IntersectionRecord& iRec, const double tMin, const double tMax) const {

	// This implementation uses Woop, Benthin and Wald's Watertight Ray/Triangle Intersection algorithm
	// as described by https://jcgt.org/published/0002/01/05/

	// Load the triangle's vertces from the array
	const Vec3f point0 = parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 0]];
	const Vec3f point1 = parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 1]];
	const Vec3f point2 = parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 2]];

	// Calculate the dimension where the ray direction is minimal
	int kz = max_element(abs(r.direction));
	int kx = kz + 1; if (kx == 3) kx = 0;
	int ky = kx + 1; if (ky == 3) ky = 0;
	
	// Swap kx and ky dimensinos to preserve triangle winding direction
	if (r.direction[kz] < 0.0f)
		std::swap(kx, ky);

	// Calculate shear constants
	float Sz = 1.0f / r.direction[kz];
	float Sx = r.direction[kx] * Sz;
	float Sy = r.direction[ky] * Sz;

	// Calculate vertex positions relative to ray origin
	const Vec3f A = point0 - r.origin;
	const Vec3f B = point1 - r.origin;
	const Vec3f C = point2 - r.origin;

	// Perform shear and scale of verts
	const float Ax = A[kx] - Sx * A[kz];
	const float Ay = A[ky] - Sy * A[kz];
	const float Bx = B[kx] - Sx * B[kz];
	const float By = B[ky] - Sy * B[kz];
	const float Cx = C[kx] - Sx * C[kz];
	const float Cy = C[ky] - Sy * C[kz];

	// Calculate scaled barycentric coords
	float U = Cx * By - Cy * Bx;
	float V = Ax * Cy - Ay * Cx;
	float W = Bx * Ay - By * Ax;

	// Perform edge tests
	if ((U < 0.0f || V < 0.0f || W < 0.0f) && (U > 0.0f || V > 0.0f || W > 0.0f))
		return false;

	if (U == 0.0f || V == 0.0f || W == 0.0f) {
		// Fallback to double precision edge tests
		double CxBy = (double)Cx * (double)By;
		double CyBx = (double)Cy * (double)Bx;
		U = (float)(CxBy - CyBx);

		double AxCy = (double)Ax * (double)Cy;
		double AyCx = (double)Ay * (double)Cx;
		V = (float)(AxCy - AyCx);

		double BxAy = (double)Bx * (double)Ay;
		double ByAx = (double)By * (double)Ax;
		W = (float)(BxAy - ByAx);

		// Perform edge tests on double precision edges
		if ((U < 0.0f || V < 0.0f || W < 0.0f) && (U > 0.0f || V > 0.0f || W > 0.0f))
			return false;
	}

	// Calculate determinant
	float det = U + V + W;
	if (det == 0.0f)
		return false;

	// Calculate scaled z-coords of verts and use to calculate hit distance
	const float Az = Sz * A[kz];
	const float Bz = Sz * B[kz];
	const float Cz = Sz * C[kz];
	float T = U * Az + V * Bz + W * Cz;

	// Compare T against range
	if (det < 0.0f && (T >= tMin * det || T < tMax * det))
		return false;
	else if (det > 0.0f && (T <= tMin * det || T > tMax * det))
		return false;

	// Normalize U, V, W and T
	const float invDet = 1.0f / det;
	U = U * invDet;
	V = V * invDet;
	W = W * invDet;
	T = T * invDet;

	// Calculate hit position
	iRec.p = r.PositionAtt(T); // U * point0 + V * point1 * W * point2;
	iRec.t = T;
	
	if (parentMesh->normalIndices.size() > 0) {
		// Normals have been provided - interpolate and calculate the shading normal
		Vec3f normal = U * parentMesh->normalPositions[parentMesh->normalIndices[3 * triangleIndex + 0]]
			+ V * parentMesh->normalPositions[parentMesh->normalIndices[3 * triangleIndex + 1]]
			+ W * parentMesh->normalPositions[parentMesh->normalIndices[3 * triangleIndex + 2]];
		iRec.normal = unit_vector(Vec3d(normal.x(), normal.y(), normal.z()));
	}
	else {
		// No normals available - calculate using cross product
		Vec3f normal = cross_product(point0 - point2, point1 - point2);
		iRec.normal = unit_vector(Vec3d(normal.x(), normal.y(), normal.z()));
	}

	iRec.light = this->light;
	iRec.mat = this->material;

	/*
	* TODO: THIS IS DISABLED UNTIL MESH-LOADED UVS ARE FIXED
	if (parentMesh->uvIndices.size() > 0) {
		// Use provided UVs
		iRec.uv = U * parentMesh->uvs[parentMesh->uvIndices[3 * triangleIndex + 0]] 
			+ V * parentMesh->uvs[parentMesh->uvIndices[3 * triangleIndex + 1]]
			+ W * parentMesh->uvs[parentMesh->uvIndices[3 * triangleIndex + 2]];
	}
	else {
		// Generate uvs
		iRec.uv = U * Vec2d(0.0, 0.0) + V * Vec2d(1.0, 0.0) + W * Vec2d(1.0, 1.0);
	}*/

	iRec.uv = U * Vec2d(0.0, 0.0) + V * Vec2d(1.0, 0.0) + W * Vec2d(1.0, 1.0);

	return iRec.mat->EvaluateAlphaTransmission(iRec);
}

double Triangle::SurfaceArea() const {
	return 0.5 * length(cross_product(
		parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 1]] - parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 0]],
		parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 2]] - parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 0]]));
}

double Triangle::SamplePDF(const Vec3d& p) const {
	// Simple 1/area
	return 1 / SurfaceArea();
}
