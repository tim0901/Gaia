#pragma once

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "object.h"
#include <cfloat>

//Ellipse defined by centre c and two radii r1 and r2

class ellipse : public object {
public:
	ellipse() {}
	ellipse(float oid, float pid, vec3 c, vec3 r1, vec3 r2, material* mat) :object_id(oid), primitive_id(pid), mat_ptr(mat), centre(c), radius1(r1), radius2(r2) {};
    ~ellipse(){
        //std::cout << "Delete ellipse" << std::endl;
        if(mat_ptr){
            //delete mat_ptr;
        }
    }
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

#endif // !ELLIPSE_H
