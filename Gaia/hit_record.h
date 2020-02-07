#pragma once

#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "vec3.h"
#include "material.h"

//Stores information about the object the ray just hit

struct hit_record {
	float object_id = -1;
	float primitive_id = -1;
	float t; //t from p(t) = A + t*B
	vec3 p; //3D position of ray hit
	float u, v;
	vec3 normal;//Normal vector of hit point
	material *mat_ptr;//A pointer to the material of the object hit
    std::string type = "hit_record";
};

#endif // !HIT_RECORD_H
