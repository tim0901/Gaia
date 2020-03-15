#pragma once

#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "random.h"
#include "object.h"

class bvh_node : public object {
public:
	bvh_node() {}
	bvh_node(object **l, int n, float time0, float time1);
    ~bvh_node(){
        //std::cout << "Delete node: ";
        if(left && left->type != "triangle"){
            //std::cout << "Delete Left" << left->type << std::endl;
            delete left;
        }
        if(right && right->type != "triangle"){
            //std::cout << "Delete Right" << right->type << std::endl;
            delete right;
        }
    }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	float object_id = -2;
	float primitive_id = -2;
	object *left;
	object *right;
	aabb box;
    
    std::string type = "bvh_node";
    
};

int box_x_compare(const void * a, const void * b);
int box_y_compare(const void * a, const void * b);
int box_z_compare(const void * a, const void * b);

#endif //BVH_H
