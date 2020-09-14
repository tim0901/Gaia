//
//  translate.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "translate.h"

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    ray moved_ray(r.origin() - displacement, r.direction());
    if (obj_ptr->hit(moved_ray, t_min, t_max, rec)) {
        rec.p += displacement;
        return true;
    }
    else {
        return false;
    }
}

bool translate::bounding_box(float t0, float t1, aabb& box) const {

    //if the object has a bounding box, move it. Otherwise don't even try.
    if (obj_ptr->bounding_box(t0, t1, box)) {
        box = aabb(box.min() + displacement, box.max() + displacement);
        return true;
    }
    else {
        return false;
    }
}
