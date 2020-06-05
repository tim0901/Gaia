#pragma once

#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "object.h"

class translate : public object {
public:
	translate(object* ptr, const vec3& disp) : obj_ptr(ptr), displacement(disp) {}
    ~translate(){
        //std::cout << "Delete translate" << std::endl;
        delete obj_ptr;
    }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

    float object_id = -3;
    float primitive_id = -3;
    
	object* obj_ptr;
	vec3 displacement;
    std::string type = "translate";
};
#endif // !TRANSLATE_H
