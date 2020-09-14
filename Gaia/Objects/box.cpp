//
//  box.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "box.h"
#include "plane.h"

box::box(float oid, const vec3& p0, const vec3& p1, material* mat_ptr):object_id(oid){
    pmin = p0;
    pmax = p1;
    object** list = new object*[6];

    int i = 0;
    int pid = 0;

    list[i++] = new xy_rect(oid, pid++, p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), vec3(0,0,-1), mat_ptr);
    list[i++] = new xy_rect(oid, pid++, p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), vec3(0,0, 1), mat_ptr);

    list[i++] = new xz_rect(oid, pid++, p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), vec3(0,1,0), mat_ptr);
    list[i++] = new xz_rect(oid, pid++, p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), vec3(0,-1,0), mat_ptr);

    list[i++] = new yz_rect(oid, pid++, p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), vec3(1,0,0), mat_ptr);
    list[i++] = new yz_rect(oid, pid++, p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), vec3(-1,0,0), mat_ptr);

    list_ptr = new object_list(list, i);
}

bool box::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    return list_ptr->hit(r, t0, t1, rec);
}
