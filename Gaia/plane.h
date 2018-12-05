#pragma once

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "object.h"

//a,b,c,d defined clockwise
class rectangle : public object {
public:
	rectangle() {}
	rectangle(vec3 a, vec3 b, vec3 c, material *mat) : mat_ptr(mat), pointa(a), pointb(b), pointc(c) {};
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record &rec) const;

	vec3 pointa;
	vec3 pointb;
	vec3 pointc;
	material *mat_ptr;
};

//Ray-rectangle intersection test
bool rectangle::hit(const ray& r, float tmin, float tmax, hit_record &rec) const {

	//Find surface normal
	vec3 normal = cross(pointb - pointa, pointa - pointc);

	//Check that ray is going in the right direction
	float intersection = dot(r.direction(), normal);
	if (intersection > 0) {
		//No intersection with plane
		return false;
	}

	//Find point of collision along ray
	float t = dot(normal, pointb - r.origin())/dot(normal,r.direction());
	
	//Rejects out of bounds collisions
	if (t <= 0 || t > FLT_MAX) {
		return false;
	}

	//find point
	vec3 p = r.p_at_t(t);

	//Vectors of sides of rectangle
	vec3 v1 = (pointb - pointa);
	vec3 v2 = (pointb - pointc);
	
	//Checks point is on the rectangle
	if (dot(v1, pointb-p) < 0 || dot(v1, pointb - p)/v1.length() > v1.length()) {
		return false;
	}
	if (dot(v2, pointb-p) < 0 || dot(v2, pointb - p)/v2.length() > v2.length()) {
		return false;
	}
	
	//Find UVs
	float u = (pointb - p).length() / v1.length();
	float v = (pointb - p).length() / v2.length();

	rec.u = u;
	rec.v = v;
	rec.t = t;
	rec.mat_ptr = mat_ptr;
	rec.p = p;
	rec.normal = normal;
	return true;
}

#endif // !RECTANGLE_H
