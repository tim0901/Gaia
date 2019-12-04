#pragma once

#ifndef RANDOM_H
#define RANDOM_H

#include <random>

#include "vec3.h"

//All random distribution functions

inline float drand48() {

	return (rand() / (RAND_MAX + 1.0));
}

inline vec3 random_to_sphere(float radius, float distance_squared) {
	float r1 = drand48();
	float r2 = drand48();
	float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	float phi = 2 * M_PI * r1;
	float x = cos(phi) * sqrt(1 - z * z);
	float y = sin(phi) * sqrt(1 - z * z);
	return vec3(x, y, z);
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

//Normalize random in unit sphere to sample on a unit sphere
vec3 randon_on_unit_sphere() {
	return unit_vector(random_in_unit_sphere());
}

#endif //RANDOM_H