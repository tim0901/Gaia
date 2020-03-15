//
//  object_list.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "pch.h"

#include "object_list.h"

float object_list::pdf_value(const vec3& o, const vec3& v) const {
    float weight = 1.0 / list_size;
    float sum = 0;
    for (int i = 0; i < list_size; i++) {
        sum += weight * list[i]->pdf_value(o, v);
    }
    return sum;
}

vec3 object_list::random(const vec3& o) const {
    int index = int(drand48() * list_size);
    return list[index]->random(o);
}

//Directs ray through list of objects if hit
bool object_list::hit(const ray& r, float t_min, float t_max, hit_record &rec) const {
    hit_record temp_record;
    bool hit_anything = false;
    double closest_so_far = t_max;
    //Iterates through list, returns closest
    for (int i = 0; i < list_size; i++) {
        if (list[i]->hit(r, t_min, closest_so_far, temp_record)) {
            hit_anything = true;
            if (temp_record.t < closest_so_far) {
                closest_so_far = temp_record.t;
                rec = temp_record;
            }

        }
    }
    return hit_anything;

}


//Creates a bounding box surrounding a list of objects
bool object_list::bounding_box(float t0, float t1, aabb &box) const {
    
    //Requires objects!
    if (list_size < 1) return false;
    
    aabb temp_box;

    bool first_true = list[0]->bounding_box(t0, t1, temp_box);
    
    //Objects need a bounding_box function
    if (!first_true) {
        return false;
    }
    else {
        box = temp_box;
    }

    //For each object in the list, check if it is within the bounding box and extend it if the object falls outside.
    for (int i = 1; i < list_size; i++) {
        if (list[0]->bounding_box(t0, t1, temp_box)) {
            box = surrounding_box(box, temp_box);
        }
        else {
            return false;
        }
    }


    //Return final bounding box
    return true;
}
