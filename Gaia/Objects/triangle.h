#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object.h"

#include "../Maths/Pi.h"
#include <opensubdiv/far/topologyDescriptor.h>
#include <opensubdiv/far/primvarRefiner.h>

class Triangle;

class Mesh {
public:
	Mesh() {}
	Mesh(const int nT, const int nV, const float s = 1.0f, const int subDivLevel = 0) :nTris(nT), nVerts(nV), scale(s), subdivisionLevel(subDivLevel) {
		tris = std::make_shared <std::vector<std::shared_ptr<Object>>>(nTris);
		vertexPositions = std::make_unique<Vec3f[]>(nVerts);
	}
	Mesh(const std::string filename, const Vec3d &position, std::vector<Material*> mats, const float s = 1.0f, const int subDivLevel = 0) : scale(s), subdivisionLevel(subDivLevel) {
		tris = std::make_shared <std::vector<std::shared_ptr<Object>>>();
		LoadMesh(filename, position, mats);
	}

	bool LoadMesh(const std::string filename, const Vec3d& position, std::vector<Material*> mats);

	std::shared_ptr<std::vector<std::shared_ptr<Object>>> tris;

	size_t nTris = 0, nVerts = 0, nUVs = 0;
	std::unique_ptr<Vec3f[]> vertexPositions;
	std::unique_ptr<Vec3f[]> normalPositions;
	std::unique_ptr<Vec2f[]> uvs;
	std::vector<int> vertexIndices;
	std::vector<int> normalIndices;
	std::vector<int> materialIndexes;
	std::vector<int> uvIndices;

private:
	float scale = 1.0f;
	int subdivisionLevel = 0;
};


class Triangle : public Object {
public:
	Triangle() : triangleIndex(0), parentMesh(nullptr) {}
	Triangle(Mesh* parent, Material* mat, uint32_t idx) : parentMesh(parent), triangleIndex(idx) { this->material = mat; }

	// Perform ray-object intersection test
	bool Intersect(Ray r, IntersectionRecord& rec, const double tMin = 0.001, const double tMax = DBL_MAX) const;

	void Sample(const Vec3d& p, IntersectionRecord& iRec) const {	}
	void Sample(IntersectionRecord& iRec) const {	}

	double SamplePDF(const Vec3d& p) const;

	double SurfaceArea() const;

	AABB BoundingBox(const double t0, const double t1) const {
		const Vec3f point0 = parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 0]];
		const Vec3f point1 = parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 1]];
		const Vec3f point2 = parentMesh->vertexPositions[parentMesh->vertexIndices[3 * triangleIndex + 2]];
		Vec3f pMin(std::min(point0.x(), std::min(point1.x(), point2.x())), std::min(point0.y(), std::min(point1.y(), point2.y())), std::min(point0.z(), std::min(point1.z(), point2.z())));
		Vec3f pMax(std::max(point0.x(), std::max(point1.x(), point2.x())), std::max(point0.y(), std::max(point1.y(), point2.y())), std::max(point0.z(), std::max(point1.z(), point2.z())));
		return AABB(pMin - 0.0001, pMax + 0.0001);
	}

	Mesh* parentMesh;
	uint32_t triangleIndex;
};



#endif // !TRIANGLE_H
