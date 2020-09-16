//
//  plane.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "plane.h"

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
    rec.type = type;
    rec.primitive = true;
    return true;
}

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
    rec.object_id = object_id;
    rec.primitive_id = primitive_id;
    rec.type = type;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;
    rec.mat_ptr = mat_ptr;
    rec.p = r.p_at_t(t);
    rec.normal = normalVector; //Should be vec3(0, 0, +/-1)
    rec.primitive = true;
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
    rec.object_id = object_id;
    rec.primitive_id = primitive_id;
    rec.type = type;
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.mat_ptr = mat_ptr;
    rec.p = r.p_at_t(t);
    rec.normal = normalVector; //Should be vec3(0, +/-1, 0)
    rec.primitive = true;
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
    rec.object_id = object_id;
    rec.primitive_id = primitive_id;
    rec.type = type;
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;
    rec.mat_ptr = mat_ptr;
    rec.p = r.p_at_t(t);
    rec.normal = normalVector; //Should be vec3(+/-1, 0, 0)
    rec.primitive = true;
    return true;
}
