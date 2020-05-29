//
//  ellipse.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "pch.h"

#include "ellipse.h"

//Ray-ellipse intersection test
bool ellipse::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - centre;

    // 1 = x^2/a^2 + y^2/b^2
    float a = (centre - radius1).squared_length();
    float b = (centre - radius2).squared_length();
    float c = dot(oc, oc) - (centre-radius1).length() * (centre-radius2).length();
    float discriminant = b - sqrt(a) * c;

//    vec3 csquared = oc * oc;

    // if(csquared - a*a * (r.direction)*(r.direction) - b*b){

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
            rec.primitive = true;
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
            rec.primitive = true;
            return true;
        }
    }
    else {
        return false;
    }
    return false;
}


bool ellipse::bounding_box(float t0, float t1, aabb& box) const {
    //box = aabb(centre - vec3(radius, radius, radius), centre + vec3(radius, radius, radius));
    return false;
}

