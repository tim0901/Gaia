#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include "random.h"
#include "onb.h"
#include <cfloat>

class sphere: public object {
public:
	sphere() {}
	sphere(float oid, float pid, vec3 c, float r, material *mat) : object_id(oid), primitive_id(pid), mat_ptr(mat), centre(c), radius(r) {};
    ~sphere(){
        //std::cout << "Delete sphere" << std::endl;
        if(mat_ptr){
            //delete mat_ptr;
        }}
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb &box) const;

	virtual float pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
			float cos_theta_max = sqrt(1 - radius * radius / (centre - o).squared_length());
			
			float solid_angle = 2 * M_PI * (1 - cos_theta_max);
			return 1 / solid_angle;
		}
		else
			return 0;
	}
	virtual vec3 random(const vec3& o) const {
		vec3 direction = centre - o;
		float distance_squared = direction.squared_length();
		onb uvw;
		uvw.build_from_w(direction);
		return uvw.local(random_to_sphere(radius, distance_squared));
	}


	float object_id;
	float primitive_id;
	float radius;
	vec3 centre;
	material *mat_ptr;
    std::string type = "sphere";
};

#endif // !SPHERE_H
