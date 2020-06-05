//
//  material.cpp
//  Gaia
//
//  Created by Alex Richardson on 15/03/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#include "pch.h"

#include "material.h"

//Describes a perfect reflection as used by metals and glasses
vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2 * dot(v, n)*n;
}

//Determines whether a ray is refracted or not
bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);
    if (discriminant > 0) {
        //Refracted
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else {
        return false;
    }

}
