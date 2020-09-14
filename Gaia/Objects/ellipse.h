#pragma once

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "pi.h"
#include "vec2.h"
#include "onb.h"
#include "random.h"
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
	virtual bool bounding_box(float t0, float t1, aabb& box) const {
		vec3 e = vec3(sqrt(radius1.x() * radius1.x() + radius2.x() * radius2.x()),
					  sqrt(radius1.y() * radius1.y() + radius2.y() * radius2.y()),
					  sqrt(radius1.z() * radius1.z() + radius2.z() * radius2.z()));
		box = aabb(centre - e, centre + e);
		return true;
	}

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

		// Generate random theta and k
		float theta = 2 * M_PI * drand48();
		float k = sqrt(drand48());

		return centre + (semiAxis1 * k * cos(theta))+ (semiAxis2 * k * sin(theta));
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
