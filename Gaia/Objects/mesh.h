#pragma once

#ifndef MESH_H
#define MESH_H

#include "pi.h"
#include <opensubdiv/far/topologyDescriptor.h>
#include <opensubdiv/far/primvarRefiner.h>

#include "image_parameters.h"
#include "object.h"
#include "bvh.h"
#include "triangle.h"
#include "tiny_obj_loader.h"

struct raw_mesh {
	float oid;
	int ntris;
	int nverts;
	vec3* vertexList;
	vec3* trianglesList;
	vec3* normalsList;
	int* materialsList;
	material** mat;
};

class mesh : public object {
public:
	mesh() {}
	//mesh(float oid, int ntris, int nverts, vec3* vertexList, vec3* trianglesList, vec3* normalsList, int* materialsList, material** mat) : object_id(oid), nTris(ntris), nVerts(nverts), vertices(vertexList), normals(normalsList), triangles(trianglesList), materials(materialsList), mat_list_ptr(mat) {
	mesh(raw_mesh* raw, vec3* ori, float s): origin(ori), scale(s){
        
        nTris = raw->ntris;
        
		pmin = pmax = raw->vertexList[0];

		for (int i = 1; i < raw->nverts; i++) {


			pmin = vec3(std::min(raw->vertexList[i].x(), pmin.x()), std::min(raw->vertexList[i].y(), pmin.y()), std::min(raw->vertexList[i].z(), pmin.z()));
			pmax = vec3(std::max(raw->vertexList[i].x(), pmax.x()), std::max(raw->vertexList[i].y(), pmax.y()), std::max(raw->vertexList[i].z(), pmax.z()));
			
		}
		
		if (!origin) {
			//No defined origin - set to centre point of mesh.
			origin = new vec3(pmin.x() + (pmax.x() - pmin.x())*0.5, pmin.y() + (pmax.y() - pmin.y())*0.5, pmin.z() + (pmax.z() - pmin.z())*0.5);
		}

        triList = new object*[nTris];


		//all points are defined wrt the origin

		int j = 0;
		for (j=j; j < nTris; j++) {

			int x = raw->trianglesList[j].x();
			int y = raw->trianglesList[j].y();
			int z = raw->trianglesList[j].z();

			//triList[j] = new triangle(raw->oid, j, (raw->vertexList[x] - *origin) * scale, (raw->vertexList[y] - *origin) * scale, (raw->vertexList[z] - *origin) * scale, raw->mat[raw->materialsList[j]], raw->normalsList[x], raw->normalsList[y], raw->normalsList[z]);
			triList[j] = new triangle(raw->oid, j, (raw->vertexList[x] - *origin) * scale, (raw->vertexList[y] - *origin) * scale, (raw->vertexList[z] - *origin) * scale, raw->mat[raw->materialsList[j]], vec3(0,0,0));
            triList[j]->type = "triangle";
            
		}

        //Free resources - we don't need the raw mesh anymore
		free(raw->vertexList);
        free(raw->normalsList);
		free(raw->trianglesList);
		free(raw->materialsList);
        free(raw->mat);
        
		std::cout << "BVH construction start." << std::endl;
		std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

		// Create bvh for mesh
		list_ptr = bvh::constructBVH(triList, j, 0, 0, SAH);

		std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
		std::chrono::duration<double> diff = end - start;
		//list_ptr = new bvh_node(triList, j, 0, 0);
		std::cout << "BVH construction complete in "<< std::chrono::duration <double, std::milli>(diff).count() << "ms." << std::endl;
		std::cout << "Mesh loaded." << std::endl;
	};
    ~mesh(){
        //std::cout << "Delete mesh" << std::endl;
        if(origin){
            delete origin;
        }
        if(list_ptr){
            delete list_ptr;
        }
        if(triList){
            //int numDeleted = 0;
            for(int i = 0; i < nTris; i++){
				//std::cout << i;
                //delete triList[i];
                //numDeleted++;
            }
            delete[] triList;
            //std::cout << numDeleted << " objects deleted." << std::endl;
        }
    }
    
	virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

    int nTris;
	float object_id;
	float primitive_id = -1; //A mesh is not a primitive
    object** triList;
	vec3* origin;
	float scale;
	object* list_ptr;
	vec3 pmin;
	vec3 pmax;
    std::string type = "mesh";
};

struct Vertex {

	// Minimal required interface ----------------------
	Vertex() { }

	Vertex(Vertex const& src) {
		_position[0] = src._position[0];
		_position[1] = src._position[1];
		_position[2] = src._position[2];
	}

	void Clear(void* = 0) {
		_position[0] = _position[1] = _position[2] = 0.0f;
	}

	void AddWithWeight(Vertex const& src, float weight) {
		_position[0] += weight * src._position[0];
		_position[1] += weight * src._position[1];
		_position[2] += weight * src._position[2];
	}

	// Public interface ------------------------------------
	void SetPosition(float x, float y, float z) {
		_position[0] = x;
		_position[1] = y;
		_position[2] = z;
	}

	const float* GetPosition() const {
		return _position;
	}

private:
	float _position[3];
};

raw_mesh load_mesh(int oid, image_parameters* image, std::string input_file, material** mat);

#endif // !MESH_H
