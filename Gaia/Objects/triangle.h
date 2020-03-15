#pragma once

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vec2.h"
#include "random.h"
#include "object.h"
#include <cfloat>

class mesh;

class triangle :public object {
public:
	triangle() {}
    triangle(float oid, float pid, vec3 p0, vec3 p1, vec3 p2, material *m, vec3 n0, vec3 n1, vec3 n2) :object_id(oid), primitive_id(pid), point0(p0), point1(p1), point2(p2), mat_ptr(m), normal0(n0), normal1(n1), normal2(n2) {}
    ~triangle(){
        //std::cout << "Delete tri" << std::endl;
        if(mat_ptr){
            //delete mat_ptr;
        }
        
    }
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	
	virtual float pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {			

			float area = cross((point1 - point0), (point2 - point0)).length()*0.5;

			float distance_squared = rec.t * rec.t * v.squared_length();
			float cosine = fabs(dot(v, rec.normal) / v.length());
			//std::cout << *point0 << " " << *point1 << " " << *point2 << " " << ((*point1 - *point0).length()) << " " << ((*point2 - *point0).length()) << " " << area << std::endl;
			return distance_squared / (cosine * area);
		}
		else {
			return 0;
		}
	}
	virtual vec3 random(const vec3& o) const {

		float a = drand48();
		float b = drand48();
		float su0 = sqrt(a);

		vec3 random_point = point0 + (1-su0)*(point1 - point0) + (b*su0)*(point2 - point0);

		return random_point - o;
		
	}

	float object_id;
	float primitive_id;
	vec3 point0;
	vec3 point1;
	vec3 point2;
	vec3 normal0;
	vec3 normal1;
	vec3 normal2;
	material *mat_ptr;
    std::string type = "triangle";
};

#endif // !TRIANGLE_H
