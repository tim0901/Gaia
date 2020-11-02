#pragma once

#ifndef LAMBERT_H
#define LAMBERT_H

#include "material.h"
#include "random.h"

class lambertian :public material {
public:
	lambertian(const vec3& a) :albedo(new solid_colour(a)) {}
	lambertian(texture* a ) :albedo(a) {}
	~lambertian() { delete albedo; }
	//Describes how a ray is scattered
	float scattering_pdf(const ray &incident, const hit_record &rec, const ray &scattered) const {
		float cosine = dot(rec.normal, unit_vector(scattered.direction()));
		if (cosine < 0) {
			cosine = 0;
			return cosine;
		}
		return cosine/M_PI;
	}
	
	virtual vec3 emitted(const ray &incident, const hit_record &rec, float u, float v, const vec3& p) const {
		
		return vec3(0, 0, 0); 
	}

	virtual bool scatter(const ray &incident, const hit_record &rec, scattering_record &scatter) const {
		scatter.is_specular = false;
        scatter.specular_ray = scatter.specular_ray;
		scatter.brdf = albedo->value(vec2(rec.u, rec.v), rec.p);
		scatter.pdf = new cosine_pdf(rec.normal);
		return true;
	}

	std::string type = "lambertian";

	texture* albedo;
};

#endif // !LAMBERT_H
