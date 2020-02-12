#pragma once

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "object.h"

//Ellipse defined by centre c and two radii r1 and r2

class ellipse : public object {
public:
	ellipse() {}
	ellipse(float oid, float pid, vec3 c, vec3 r1, vec3 r2, material* mat) :object_id(oid), primitive_id(pid), mat_ptr(mat), centre(c), radius1(r1), radius2(r2) {};
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	virtual float pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
			float area = M_PI * (radius1 - centre).length() * (radius2 - centre).length();
			float distance_squared = rec.t * rec.t * v.squared_length();
			float cosine = fabs(dot(v, rec.normal) / v.length());
			return  distance_squared / (cosine * area);
		}
		else
			return 0;
	}

	virtual vec3 random(const vec3& o) const {
		vec3 semiAxis1 = radius1 - centre; //Find semimajor/semiminor axes
		vec3 semiAxis2 = radius2 - centre;
		vec3 p;
		do {
			centre - (2 - drand48()) * semiAxis1 - (2 - drand48()) * semiAxis2;
		} while (sqrt(semiAxis1.length() * semiAxis1.length() + semiAxis2.length() * semiAxis2.length()) > 1);

		return p;
	}

	float object_id;
	float primitive_id;
	vec3 radius1;
	vec3 radius2;
	vec3 centre;
	material* mat_ptr;
	std::string type = "ellipse";
};

//Ray-ellipse intersection test
bool ellipse::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
	vec3 oc = r.origin() - centre; 

	// 1 = x^2/a^2 + y^2/b^2
	float a = (centre - radius1).squared_length();
	float b = (centre - radius2).squared_length();
	float c = dot(oc, oc) - (centre-radius1).length() * (centre-radius2).length();
	float discriminant = b - sqrt(a) * c;

	vec3 csquared = oc * oc;

	if(csquared - a*a * (r.direction)*(r.direction) - b*b){

	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.object_id = object_id;
			rec.primitive_id = primitive_id;
			rec.type = type;
			rec.t = temp;
			rec.p = r.p_at_t(rec.t);
			rec.normal = cross(radius1, radius2);
			rec.mat_ptr = mat_ptr;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.object_id = object_id;
			rec.primitive_id = primitive_id;
			rec.type = type;
			rec.t = temp;
			rec.p = r.p_at_t(rec.t);
			rec.normal = cross(radius1, radius2);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	else {
		return false;
	}

}


bool ellipse::bounding_box(float t0, float t1, aabb& box) const {
	//box = aabb(centre - vec3(radius, radius, radius), centre + vec3(radius, radius, radius));
	return false;
}


#endif // !ELLIPSE_H
