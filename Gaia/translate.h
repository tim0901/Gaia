#pragma once

#ifndef TRANSLATE_H
#define TRANSLATE_H

<<<<<<< HEAD
#include "object.h"
=======
#include "Objects/object.h"
>>>>>>> Nightly

class translate : public object {
public:
	translate(object* ptr, const vec3& disp) : obj_ptr(ptr), displacement(disp) {}
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

<<<<<<< HEAD
	object* obj_ptr;
	vec3 displacement;
=======
    float object_id = -3;
    float primitive_id = -3;
    
	object* obj_ptr;
	vec3 displacement;
    std::string type = "translate";
>>>>>>> Nightly
};

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	ray moved_ray(r.origin() - displacement, r.direction());
	if (obj_ptr->hit(moved_ray, t_min, t_max, rec)) {
		rec.p += displacement;
		return true;
	}
	else {
		return false;
	}
}

bool translate::bounding_box(float t0, float t1, aabb& box) const {

	//if the object has a bounding box, move it. Otherwise don't even try.
	if (obj_ptr->bounding_box(t0, t1, box)) {
		box = aabb(box.min() + displacement, box.max() + displacement);
		return true;
	}
	else {
		return false;
	}
}


#endif // !TRANSLATE_H
