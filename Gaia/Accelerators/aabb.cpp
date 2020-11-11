//
//  aabb.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "aabb.h"

//Returns a bounding box that surrounds two bounding boxes given to it.
aabb surrounding_box(const aabb &box0, const aabb &box1) {

    vec3 small(fmin(box0.min().x(), box1.min().x()), fmin(box0.min().y(), box1.min().y()), fmin(box0.min().z(), box1.min().z()));

    vec3 big(fmax(box0.max().x(), box1.max().x()), fmax(box0.max().y(), box1.max().y()), fmax(box0.max().z(), box1.max().z()));

    return aabb(small, big);
}

// Extend a bounding box to include a point.
aabb surrounding_box(const aabb &box0, const vec3 &point) {
    
    vec3 small(fmin(box0.min().x(), point.x()), fmin(box0.min().y(), point.y()), fmin(box0.min().z(), point.z()));

    vec3 big(fmax(box0.max().x(), point.x()), fmax(box0.max().y(), point.y()), fmax(box0.max().z(), point.z()));

    return aabb(small, big);
}