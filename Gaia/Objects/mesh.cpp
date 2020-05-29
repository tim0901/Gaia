//
//  mesh.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "pch.h"

#include "mesh.h"

bool mesh::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(pmin, pmax);
    return true;
}


bool mesh::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    return list_ptr->hit(r, t0, t1, rec);
}

raw_mesh load_mesh(int oid, image_parameters* image, std::string input_file, material** mat) {

    std::string inputfile = input_file;

    std::cout << "Loading: " << input_file << std::endl;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str(), 0, true);

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    //std::cout << "Verts: " << attrib.vertices.size() << std::endl;
    //std::cout << "Normals: " << attrib.normals.size() << std::endl;
    //std::cout << "Shapes: " << shapes.size() << std::endl;

    //Allocate space in memory for vectors.

    vec3* vertexList = { 0 };
    vertexList = (vec3*)calloc(attrib.vertices.size(), sizeof(vec3));

    vec3* normalsList = { 0 };
    normalsList = (vec3*)calloc(attrib.normals.size(), sizeof(vec3));

    vec3* trianglesList = { 0 };
    trianglesList = (vec3*)calloc(attrib.vertices.size()/3.0, sizeof(vec3));

    static int* indiciesList = { 0 };
    indiciesList = (int*)calloc(attrib.normals.size(), sizeof(int));

    int* materialsList = { 0 };
    materialsList = (int*)calloc(attrib.vertices.size(), sizeof(int));


    //    std::cout << shapes[0].mesh.material_ids[0] << std::endl;
        /*
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                vec3 vertex = {};



                vertex = vec3(attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                );

                //vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }*/
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            vertexList[index.vertex_index] = vec3(attrib.vertices[3.0 * index.vertex_index], attrib.vertices[3.0 * index.vertex_index + 1.0], attrib.vertices[3.0 * index.vertex_index + 2.0]);

        }
    }

    if (attrib.normals.size() != 0) {
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                normalsList[index.normal_index] = vec3(attrib.normals[3.0 * index.normal_index], attrib.normals[3.0 * index.normal_index + 1.0], attrib.normals[3.0 * index.normal_index + 2.0]);
            }
        }
    }
    /*
    std::cout << attrib.vertices.size() << std::endl;

    for (int i = 0; i < attrib.vertices.size()-1; i++) {
        vertexList[i] = vec3(attrib.vertices[3 * i], attrib.vertices[3 * i + 1], attrib.vertices[3 * i + 2]);
        //normalsList[i] = vec3(attrib.normals[3 * i], attrib.normals[3 * i + 1], attrib.normals[3 * i + 2]);
    }*/

    int nTris = 0;
    for (const auto& shape : shapes) {

        for (int i = 0; i < shape.mesh.indices.size(); i++) {
            indiciesList[nTris++] = shape.mesh.indices[i].vertex_index;
            //std::cout << indiciesList[i] << std::endl;
        }
    }
    //Calculate number of triangles in the mesh
    nTris = nTris / 3;

    //Extract old materials data from shape, stored in materialsList
    for (int i = 0; i < nTris; i++) {
        //Load the associated material for the triangle

        if (nTris > shapes[0].mesh.material_ids.size()) {
            materialsList[i] = 0;
        }
        else {
            materialsList[i] = shapes[0].mesh.material_ids[i];
        }
    }

    std::cout << "Subdividing mesh." << std::endl;

    using namespace OpenSubdiv;

    typedef Far::TopologyDescriptor Descriptor; //nickname for ease of use

    Sdc::SchemeType type = OpenSubdiv::Sdc::SCHEME_LOOP;

    Sdc::Options options;
    options.SetVtxBoundaryInterpolation(Sdc::Options::VTX_BOUNDARY_EDGE_ONLY);//Set interpolation options

    int nVerts = int(attrib.vertices.size());

    static int* vertsperfacelist = { 0 };
    int** vertsperfacelistPtr = &vertsperfacelist;
    *vertsperfacelistPtr = (int*)calloc(1000000, sizeof(int));

    for (int i = 0; i < nTris; i++) {
        vertsperfacelist[i] = 3;
    }

    Descriptor desc;
    desc.numVertices = nVerts;
    desc.numFaces = nTris;
    desc.numVertsPerFace = vertsperfacelist;
    desc.vertIndicesPerFace = indiciesList;

    // Instantiate a FarTopologyRefiner from the descriptor
    Far::TopologyRefiner* refiner = Far::TopologyRefinerFactory<Descriptor>::Create(desc, Far::TopologyRefinerFactory<Descriptor>::Options(type, options));

    // Uniformly refine the topology up to 'maxlevel'
    refiner->RefineUniform(Far::TopologyRefiner::UniformOptions(image->maxlevel));

    // Allocate a buffer for vertex primvar data. The buffer length is set to
    // be the sum of all children vertices up to the highest level of refinement.
    std::vector<Vertex> vbuffer(refiner->GetNumVerticesTotal());
    Vertex* verts = &vbuffer[0];

    // Initialize coarse mesh positions
    
    for (int i = 0; i < nVerts; ++i) {
        verts[i].SetPosition(vertexList[i].x(), vertexList[i].y(), vertexList[i].z());
    }


    //Create temporary materials list for interpolation
    int* tempmatlist = { 0 };
    tempmatlist = (int*)calloc(nTris, sizeof(int));

    // Interpolate vertex primvar data
    Far::PrimvarRefiner primvarRefiner(*refiner);

    Vertex* src = verts;
    for (int level = 1; level <= image->maxlevel; ++level) {
        Vertex* dst = src + refiner->GetLevel(level - 1).GetNumVertices();
        primvarRefiner.Interpolate(level, src, dst);
        src = dst;

        Far::TopologyLevel const& refCurrentLevel = refiner->GetLevel(level);

        //Extract number of triangles in the current level
        nTris = refCurrentLevel.GetNumFaces();

        materialsList = (int*)realloc(materialsList, nTris * sizeof(int));
        tempmatlist = (int*)realloc(tempmatlist, nTris * sizeof(int));
		normalsList = (vec3*)realloc(normalsList, nTris * sizeof(vec3)); //The normals list is resized, but the new normals are not calculated. TODO.

        //Extract per-face materials
        for (int i = 0; i < nTris; i++) {

            //Find material for each face from the face in the previous iteration that the new face corresponds to
            tempmatlist[i] = materialsList[refCurrentLevel.GetFaceParentFace(i)];

        }
        for (int i = 0; i < nTris; i++) {

            materialsList[i] = tempmatlist[i];
        }
    }
    
    //Extract last level of refined topology
    Far::TopologyLevel const& refLastLevel = refiner->GetLevel(image->maxlevel);
    nVerts = refLastLevel.GetNumVertices();

    //Iterator
    int firstOfLastVerts = refiner->GetNumVerticesTotal() - nVerts;

    std::cout << "Verts: " << nVerts << ". Tris: " << nTris << "." << std::endl;

    //Resize arrays for larger mesh
    vertexList = (vec3*)realloc(vertexList, nVerts * sizeof(vec3));
    trianglesList = (vec3*)realloc(trianglesList, nTris * sizeof(vec3));

    //Output vertices to array
    for (int i = 0; i < nVerts; i++) {
        float const* pos = verts[firstOfLastVerts + i].GetPosition();
        vertexList[i] = vec3(pos[0], pos[1], pos[2]);
    }
    
    //Output faces to array
    for (int i = 0; i < nTris; i++) {
        Far::ConstIndexArray fverts = refLastLevel.GetFaceVertices(i);
        trianglesList[i] = vec3(fverts[0], fverts[1], fverts[2]);
    }
    
    //delete refiner;
    attrib.colors.clear();
    attrib.normals.clear();
    attrib.texcoords.clear();
    attrib.vertices.clear();
    free(indiciesList);
    materials.clear();
    shapes.clear();
    vbuffer.clear();
    free(tempmatlist);
    free(vertsperfacelist);
    
    std::cout << "Subdivision complete." << std::endl;

    raw_mesh* raw = new raw_mesh;
    raw->oid = oid;
    raw->ntris= nTris;
    nTris = 0;
    raw->nverts = nVerts;
    nVerts = 0;
    raw->vertexList = vertexList;
    raw->trianglesList = trianglesList;
    raw->normalsList = normalsList;
    raw->materialsList = materialsList;
    raw->mat = mat;
    
    return *raw;

    //return raw_mesh(oid, nTris, nVerts, vertexList, trianglesList, normalsList, materialsList, mat);
}
