//
//  onb.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "pch.h"

#include "onb.h"

void onb::build_from_w(const vec3& n) {
    axis[2] = unit_vector(n);
    vec3 a;
    if (fabs(w().x()) > 0.9) {
        a = vec3(0, 1, 0);
    }
    else {
        a = vec3(1, 0, 0);
    }
    axis[1] = unit_vector(cross(w(), a));
    axis[0] = cross(w(), v());
}
