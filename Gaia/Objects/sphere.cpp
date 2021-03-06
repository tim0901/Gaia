//
//  sphere.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include"sphere.h"


void getSphereUV(const vec3& p, float& u, float& v) {
    double phi = atan2(p.z(), p.x());
    double theta = asin(p.y());
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}


//Ray-sphere intersection test
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record &rec) const {

    vec3 oc = r.origin() - centre;
    //Calculate discriminant using quadratic formula
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b * b - a * c;
    
    //Test for intersection
    rec.numberOfIntersectionTests++; // This is a primitive
    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.object_id = object_id;
            rec.primitive_id = primitive_id;
            rec.t = temp;
            rec.p = r.p_at_t(rec.t);
            rec.normal = (rec.p - centre) / radius;
            rec.mat_ptr = mat_ptr;
            getSphereUV((rec.p - centre) / radius, rec.u, rec.v);
            rec.type = type;
            rec.primitive = true;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if (temp < t_max && temp > t_min) {
            rec.object_id = object_id;
            rec.primitive_id = primitive_id;
            rec.t = temp;
            rec.p = r.p_at_t(rec.t);
            rec.normal = (rec.p - centre) / radius;
            rec.mat_ptr = mat_ptr;
            getSphereUV((rec.p - centre) / radius, rec.u, rec.v);
            rec.type = type;
            rec.primitive = true;
            return true;
        }
    }
    return false;
}

bool sphere::bounding_box(float t0, float t1, aabb &box) const{
    box = aabb(centre - vec3(radius, radius, radius), centre + vec3(radius, radius, radius));
    return true;
}
