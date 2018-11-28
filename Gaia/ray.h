#pragma once

#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
	ray() {}
	ray(const vec3& a, const vec3& b) { A = a; B = b; }
	vec3 origin() const { return A; }
	vec3 direction() const { return B; }
	
	//p_at_t returns the solution of P(t) = A + tB
	vec3 p_at_t(float t) const { return A + t * B; }

	vec3 A, B;
};

#endif // !RAY_H
