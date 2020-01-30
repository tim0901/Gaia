#pragma once

#ifndef LAMBERT_H
#define LAMBERT_H

#include "material.h"
#include "random.h"
#include "onb.h"

class lambertian :public material {
public:
	lambertian(const vec3& a) :albedo(a) {}
	//Describes how a ray is scattered
	float scattering_pdf(const ray &incident, const hit_record &rec, const ray &scattered) const {
		float cosine = dot(rec.normal, unit_vector(scattered.direction()));
		if (cosine < 0) {
			cosine = 0;
			return 0;
		}
		return cosine/M_PI;
	}
	
	virtual vec3 emitted(const ray &incident, const hit_record &rec, float u, float v, const vec3& p) const {
		
		return vec3(0, 0, 0); 
	}

	virtual bool scatter(const ray &incident, const hit_record &rec, scattering_record &scatter) const {
		scatter.is_specular = false;
        scatter.specular_ray;
		scatter.brdf = albedo;
		scatter.pdf = new cosine_pdf(rec.normal);
		return true;
	}

	std::string type = "lambertian";

	vec3 albedo;
};

#endif // !LAMBERT_H
