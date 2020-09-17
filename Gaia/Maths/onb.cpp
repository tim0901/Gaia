//
//  onb.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "onb.h"

void onb::build_from_w(const vec3& n) {
    vec3 norm = unit_vector(n);
    vec3 a;
    if (fabs(norm.x()) > 0.9) {
        a = vec3(0, 1, 0);
    }
    else {
        a = vec3(1, 0, 0);
    }
    vec3 axis1 = unit_vector(cross(norm, a));
    axes = matrix3(cross(norm, axis1), axis1, norm);

    inverseAxes = axes.returnTranspose();
}

void onb::build_from_w_with_a(const vec3& n, const vec3& a) {
    vec3 norm = unit_vector(n);
    vec3 axis1 = unit_vector(cross(norm, a));
    axes = matrix3(cross(norm, axis1), axis1, norm);

    inverseAxes = axes.returnTranspose();
}
