#pragma once

#ifndef ROTATE_H
#define ROTATE_H

#include <cfloat>
#include "pi.h"
#include "object.h"


class rotate_x : public object {
public:
	rotate_x(object* p, float angle);
    ~rotate_x(){
        //std::cout << "Delete rotate_x" << std::endl;
        delete obj_ptr;
    }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = bbox; return hasbox;
	}
    
    float object_id = -4;
    float primitive_id = -4;
    
	object* obj_ptr;
	float sin_theta;
	float cos_theta;
	bool hasbox;
	aabb bbox;
    std::string type = "rotate";
};

class rotate_y: public object {
public:
	rotate_y(object* p, float angle);
    ~rotate_y(){
        //std::cout << "Delete rotate_y" << std::endl;
        delete obj_ptr;
    }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = bbox;
		return hasbox;
	}

	object* obj_ptr;
	float sintheta;
	float costheta;
	bool hasbox;
	aabb bbox;
};

class rotate_z : public object {
public:
	rotate_z(object* p, float angle);
    ~rotate_z(){
        //std::cout << "Delete rotate_z" << std::endl;
        delete obj_ptr;
    }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = bbox; return hasbox;
	}
	object* obj_ptr;
	float sin_theta;
	float cos_theta;
	bool hasbox;
	aabb bbox;
};

#endif // !ROTATE_H
