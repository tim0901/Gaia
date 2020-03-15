#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec3.h"
#include "ray.h"

//Forward declaration
class pdf;

class material;

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


struct scattering_record {
	ray specular_ray;
	bool is_specular;
	vec3 brdf;
	pdf* pdf;
};

//Defines a material
class material {
public:
    virtual ~material(){}
	//Scatter function
	virtual bool scatter(const ray& r_in, const hit_record& rec, scattering_record &scatter) const = 0;
	virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const { return vec3(0, 0, 0); }
	virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const { return 0; }

	std::string type = "undefined";

};

//Describes a perfect reflection as used by metals and glasses
vec3 reflect(const vec3 &v, const vec3 &n);

//Determines whether a ray is refracted or not
bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3 &refracted);

#endif // !MATERIAL_H
