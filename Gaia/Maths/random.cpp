//
//  random.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "pch.h"

#include "random.h"

//Uniform random sampling of a unit sphere
vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.0*vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
    } while (p.squared_length() >= 1.0);
    return p;
}

//Normalize random in unit sphere to sample on a unit sphere
vec3 randon_on_unit_sphere() {
    return unit_vector(random_in_unit_sphere());
}
