#pragma once

#ifndef BOX_H
#define BOX_H

#include "object.h"
#include "object_list.h"

class box :public object {
public:
	box(){}
	box(float oid, const vec3& p0, const vec3& p1, material* mat_ptr);
    ~box(){
        //std::cout << "Delete box" << std::endl;
        if(list_ptr){
            delete list_ptr;
        }
    }
	virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(pmin, pmax);
		return true;
	}

	float object_id;
	vec3 pmin, pmax;
	object* list_ptr;
    std::string type = "box";
};

#endif // !BOX_H
