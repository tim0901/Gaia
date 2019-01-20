#pragma once

#ifndef PLANE_H
#define PLANE_H

#include "object.h"

//Defines non axis-aligned quads

//a,b,c,d defined clockwise
class rectangle : public object {
public:
	rectangle() {}
	rectangle(float oid, float pid, vec3 a, vec3 b, vec3 c, material *mat) : object_id(oid), primitive_id(pid), mat_ptr(mat), pointa(a), pointb(b), pointc(c) {};
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		return false;
	}

	float object_id;
	float primitive_id;
	vec3 pointa;
	vec3 pointb;
	vec3 pointc;
	material *mat_ptr;
};

//Ray-rectangle intersection test
bool rectangle::hit(const ray& r, float t_min, float t_max, hit_record &rec) const {

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

	rec.object_id = object_id;
	rec.primitive_id = primitive_id;
	rec.u = u;
	rec.v = v;
	rec.t = t;
	rec.mat_ptr = mat_ptr;
	rec.p = p;
	rec.normal = normal;
	return true;
}


//Defines axis-aligned planes - more efficient but less flexible in use.

class xy_rect :public object {
public:
	xy_rect() {}
	xy_rect(float oid, float pid, float x_0, float x_1, float y_0, float y_1, float con, vec3 n, material *mat) : object_id(oid), primitive_id(pid), x0(x_0), x1(x_1), y0(y_0), y1(y_1), const_axis(con), normal(n), mat_ptr(mat) {};
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(x0, y0, const_axis - 0.0001), vec3(x1, y1, const_axis + 0.0001));
		return true;
	}

	float object_id;
	float primitive_id;

	material *mat_ptr;
	vec3 normal;//Should be vec3(0, 0, +/-1)
	float x0, x1, y0, y1, const_axis;
};

class xz_rect :public object {
public:
	xz_rect() {}
	xz_rect(float oid, float pid, float x_0, float x_1, float z_0, float z_1, float con, vec3 n, material *mat) : object_id(oid), primitive_id(pid), x0(x_0), x1(x_1), z0(z_0), z1(z_1), const_axis(con), normal(n), mat_ptr(mat) {};
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(x0, const_axis - 0.0001, z0), vec3(x1, const_axis + 0.0001, z1));
		return true;
	}

	float object_id;
	float primitive_id;

	material *mat_ptr;
	vec3 normal;//Should be vec3(0, +/-1, 0)
	float x0, x1, z0, z1, const_axis;
};

class yz_rect :public object {
public:
	yz_rect() {}
	yz_rect(float oid, float pid, float y_0, float y_1, float z_0, float z_1, float con, vec3 n, material *mat) : object_id(oid), primitive_id(pid), y0(y_0), y1(y_1), z0(z_0), z1(z_1), const_axis(con), normal(n), mat_ptr(mat) {};
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(const_axis - 0.0001, y0, z0), vec3(const_axis + 0.0001, y1, z1));
		return true;
	}

	float object_id;
	float primitive_id;

	material *mat_ptr;
	vec3 normal;//Should be vec3(+/-1, 0, 0)
	float y0, y1, z0, z1, const_axis;
};

bool xy_rect::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {

	float t = (const_axis - r.origin().z()) / r.direction().z();
	if (t < t_min || t > t_max) {
		return false;
	}

	//Calculate intersection point of t with the plane
	float x = r.origin().x() + t * r.direction().x();
	float y = r.origin().y() + t * r.direction().y();

	//Check hit point is within desired rectangle
	if (x < x0 || x > x1 || y < y0 || y > y1) {
		return false;
	}

	//Fill in hit record
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.t = t;
	rec.mat_ptr = mat_ptr;
	rec.p = r.p_at_t(t);
	rec.normal = normal; //Should be vec3(0, 0, +/-1)
	return true;
}

bool xz_rect::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {

	float t = (const_axis - r.origin().y()) / r.direction().y();
	if (t < t_min || t > t_max) {
		return false;
	}

	//Calculate intersection point of t with the plane
	float x = r.origin().x() + t * r.direction().x();
	float z = r.origin().z() + t * r.direction().z();

	//Check hit point is within desired rectangle
	if (x < x0 || x > x1 || z < z0 || z > z1) {
		return false;
	}

	//Fill in hit record
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mat_ptr;
	rec.p = r.p_at_t(t);
	rec.normal = normal; //Should be vec3(0, +/-1, 0)
	return true;
}

bool yz_rect::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {

	float t = (const_axis - r.origin().x()) / r.direction().x();
	if (t < t_min || t > t_max) {
		return false;
	}

	//Calculate intersection point of t with the plane
	float y = r.origin().y() + t * r.direction().y();
	float z = r.origin().z() + t * r.direction().z();

	//Check hit point is within desired rectangle
	if (y < y0 || y > y1 || z < z0 || z > z1) {
		return false;
	}

	//Fill in hit record
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.t = t;
	rec.mat_ptr = mat_ptr;
	rec.p = r.p_at_t(t);
	rec.normal = normal; //Should be vec3(+/-1, 0, 0)
	return true;
}

#endif // !PLANE_H
