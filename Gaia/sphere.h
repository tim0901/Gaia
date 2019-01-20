#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"

class sphere: public object {
public:
	sphere() {}
	sphere(float oid, float pid, vec3 c, float r, material *mat) : object_id(oid), primitive_id(pid), mat_ptr(mat), centre(c), radius(r) {};
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb &box) const;


	float object_id;
	float primitive_id;
	float radius;
	vec3 centre;
	material *mat_ptr;
};

//Ray-sphere intersection test
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record &rec) const {

	vec3 oc = r.origin() - centre;
	//Calculate discriminant using quadratic formula
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b * b - a * c;
	
	//Test for intersection
	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.object_id = object_id;
			rec.primitive_id = primitive_id;
			rec.t = temp;
			rec.p = r.p_at_t(rec.t);
			rec.normal = (rec.p - centre) / radius;
			rec.mat_ptr = mat_ptr;
			rec.u = 1 - (atan2(rec.p.z(), rec.p.x()) + M_PI) / (2 * M_PI);
			rec.v = (asin(rec.p.y()) + M_PI / 2) / M_PI;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.object_id = object_id;
			rec.primitive_id = primitive_id;
			rec.t = temp;
			rec.p = r.p_at_t(rec.t);
			rec.normal = (rec.p - centre) / radius;
			rec.mat_ptr = mat_ptr;
			rec.u = 1 - (atan2(rec.p.z(), rec.p.x()) + M_PI) / (2 * M_PI);
			rec.v = (asin(rec.p.y()) + M_PI / 2) / M_PI;
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb &box) const{
	box = aabb(centre - vec3(radius, radius, radius), centre + vec3(radius, radius, radius));
	return true;
}

#endif // !SPHERE_H
