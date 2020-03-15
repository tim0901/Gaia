#pragma once

#ifndef PLANE_H
#define PLANE_H

#include "object.h"
#include <cfloat>

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
    ~rectangle(){
        //std::cout << "Delete plane" << std::endl;
        if(mat_ptr){
            //delete mat_ptr;
        }
    }

	float object_id;
	float primitive_id;
	vec3 pointa;
	vec3 pointb;
	vec3 pointc;
	material *mat_ptr;
    std::string type = "plane";
};


//Defines axis-aligned planes - more efficient but less flexible in use.

class xy_rect :public object {
public:
	xy_rect() {}
	xy_rect(float oid, float pid, float x_0, float x_1, float y_0, float y_1, float con, vec3 n, material *mat) : object_id(oid), primitive_id(pid), x0(x_0), x1(x_1), y0(y_0), y1(y_1), const_axis(con), normal(n), mat_ptr(mat) {};
    ~xy_rect(){
        //std::cout << "Delete xy_rect" << std::endl;
        if(mat_ptr){
            //delete mat_ptr;
        }
        
    }
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(x0, y0, const_axis - 0.0001), vec3(x1, y1, const_axis + 0.0001));
		return true;
	}

	virtual float pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
			float area = (x1 - x0) * (y1 - y0);
			float distance_squared = rec.t * rec.t * v.squared_length();
			float cosine = fabs(dot(v, rec.normal) / v.length());
			return  distance_squared / (cosine * area);
		}
		else
			return 0;
	}
	virtual vec3 random(const vec3& o) const {
		vec3 random_point = vec3(x0 + drand48() * (x1 - x0),
			y0 + drand48() * (y1 - y0), const_axis);
		return random_point - o;
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
    ~xz_rect(){
        //std::cout << "Delete xz_rect" << std::endl;
        if(mat_ptr){
            //delete mat_ptr;
        }
        
    }
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(x0, const_axis - 0.0001, z0), vec3(x1, const_axis + 0.0001, z1));
		return true;
	}

	virtual float pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
			float area = (x1 - x0) * (z1 - z0);
			float distance_squared = rec.t * rec.t * v.squared_length();
			float cosine = fabs(dot(v, rec.normal) / v.length());
			return  distance_squared / (cosine * area);
		}
		else
			return 0;
	}
	virtual vec3 random(const vec3& o) const {
		vec3 random_point = vec3(x0 + drand48() * (x1 - x0), const_axis,
			z0 + drand48() * (z1 - z0));
		return random_point - o;
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
    ~yz_rect(){
        //std::cout << "Delete yz_rect" << std::endl;
        if(mat_ptr){
            //delete mat_ptr;
        }
    }
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		box = aabb(vec3(const_axis - 0.0001, y0, z0), vec3(const_axis + 0.0001, y1, z1));
		return true;
	}

	virtual float pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
			float area = (y1 - y0) * (z1 - z0);
			float distance_squared = rec.t * rec.t * v.squared_length();
			float cosine = fabs(dot(v, rec.normal) / v.length());
			return  distance_squared / (cosine * area);
		}
		else
			return 0;
	}
	virtual vec3 random(const vec3& o) const {
		vec3 random_point = vec3(const_axis, y0 + drand48() * (y1 - y0),
			z0 + drand48() * (z1 - z0));
		return random_point - o;
	}

	float object_id;
	float primitive_id;

	material *mat_ptr;
	vec3 normal;//Should be vec3(+/-1, 0, 0)
	float y0, y1, z0, z1, const_axis;
};

#endif // !PLANE_H
