#pragma once

#ifndef MESH_H
#define MESH_H

#include "hitable.h"
#include "triangle.h"
#include "tiny_obj_loader.h"

class mesh : public hitable {
public:
	mesh() {}
	mesh(int ntris, int nverts, vec3* vertexList, vec3* trianglesList, vec3* normalsList, material* mat) : nTris(ntris), nVerts(nverts), vertices(vertexList), normals(normalsList), triangles(trianglesList), mat_ptr(mat) {
		
		pmin = pmax = vertices[0];

		for (int i = 1; i < nVerts; i++) {

			//X

			pmin = vec3(std::min(vertices[i].x(), pmin.x()), std::min(vertices[i].y(), pmin.y()), std::min(vertices[i].z(), pmin.z()));
			pmax = vec3(std::max(vertices[i].x(), pmax.x()), std::max(vertices[i].y(), pmax.y()), std::max(vertices[i].z(), pmax.z()));
			/*
			if (vertices[i][0] <= pmin[0]) {
				pmin[0] = vertices[i][0];
			}
			else if (vertices[i].x() > pmax.x()) {
				pmax[0] = vertices[i][0];
			}
			//Y
			if (vertices[i].y() < pmin.y()) {
				pmin[1] = vertices[i][1];
			}
			else if (vertices[i].y() > pmax.y()) {
				pmax[1] = vertices[i][1];
			}
			//Z
			if (vertices[i].z() < pmin.z()) {
				pmin[2] = vertices[i][2];
			}
			else if (vertices[i].z() > pmax.z()) {
				pmax[2] = vertices[i][2];
			}*/

		}

		hitable **list = new hitable*[nTris];

		int j = 0;
		for (j; j < nTris; j++) {

			hitable **temp = new hitable*[1];
			int x = trianglesList[j].x();
			int y = trianglesList[j].y();
			int z = trianglesList[j].z();

			list[j] = new triangle(&vertexList[x], &vertexList[y], &vertexList[z], mat, &normals[x], &normals[y], &normals[z]);
		}
		list_ptr = new bvh_node(list, j, 0, 0);
		//list_ptr = new hitable_list(list, j);
	};

	virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	int nTris;
	int nVerts;
	vec3* normals;
	vec3* triangles;
	vec3* vertices;
	material* mat_ptr;
	hitable *list_ptr; 
	vec3 pmin;
	vec3 pmax;
};

bool mesh::bounding_box(float t0, float t1, aabb& box) const{
	box = aabb(pmin, pmax);
	return true;
}


bool mesh::hit(const ray& r, float t0, float t1, hit_record& rec) const {
	return list_ptr->hit(r, t0, t1, rec);
}

mesh load_mesh(std::string input_file, material *mat) {

	std::string inputfile = input_file;

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

	vec3* vertexList = { 0 };
	vec3** vertexListPtr = &vertexList;
	*vertexListPtr = (vec3*)calloc(attrib.vertices.size(), sizeof(vec3));

	vec3* normalsList = { 0 };
	vec3** normalsListPtr = &normalsList;
	*normalsListPtr = (vec3*)calloc(attrib.normals.size(), sizeof(vec3));

	vec3* trianglesList = { 0 };
	vec3** trianglesListPtr = &trianglesList;
	*trianglesListPtr = (vec3*)calloc(1000000, sizeof(vec3));

	int* indiciesList = { 0 };
	int** indiciesListPtr = &indiciesList;
	*indiciesListPtr = (int*)calloc(1000000, sizeof(int));
	/*
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vec3 vertex = {};



			vertex = vec3(attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			);

			//vertex.color = { 1.0f, 1.0f, 1.0f };

			/*if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}*/
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vertexList[index.vertex_index] = vec3(attrib.vertices[3 * index.vertex_index], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2]);
			
		}
	}

	if (attrib.normals.size() != 0) {
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				normalsList[index.normal_index] = vec3(attrib.normals[3 * index.normal_index], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2]);
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
	nTris = nTris / 3;

	for (int i = 0; i < nTris; i++) {

		trianglesList[i] = vec3(indiciesList[3 * i], indiciesList[3 * i + 1], indiciesList[3 * i + 2]);
		//std::cout << trianglesList[i] << std::endl;
	}
	//std::cout << "Tris: " << nTris << std::endl;
	return mesh(nTris, int(attrib.vertices.size()), vertexList, trianglesList, normalsList, mat);
}
#endif // !MESH_H
