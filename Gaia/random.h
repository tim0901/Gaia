#pragma once

#ifndef RANDOM_H
#define RANDOM_H

#include <random>

#include "vec3.h"

//All random distribution functions

inline float drand48() {

	return (rand() / (RAND_MAX + 1.0));
}

//Uniform random sampling of a unit disk
inline vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0*vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}

//Uniform random sampling of a unit sphere
vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0*vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;
}


#endif //RANDOM_H