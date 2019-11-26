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
		}
		return cosine/M_PI;
	}
	
	virtual vec3 emitted(const ray &incident, const hit_record &rec, float u, float v, const vec3& p) const {
		
		return vec3(0, 0, 0); }


	virtual bool scatter(const ray &incident, const hit_record &rec, vec3 &brdf, ray &scattered, float &pdf) const {
		//New ray direction is uniformly sampled from a sphere
		vec3 target;

		/*
		//This will break as soon as the ray is travelling in the -ve direction.
		do {
			target = rec.p + rec.normal + random_in_unit_sphere();
		} while (dot(target, rec.normal) < 0);
		*/

		onb uvw;
		uvw.build_from_w(rec.normal);

		scattered = ray(rec.p, unit_vector(uvw.local(random_cosine_direction())));
		
		//Temp
		target = rec.p + rec.normal + random_in_unit_sphere();
		
		//Fires new ray
		//scattered = ray(rec.p,unit_vector(target - rec.p));


		float cos_theta = dot(scattered.direction(), rec.normal);
		
		//Absorbs a little of the colour of the material
		//brdf = cos_theta * albedo/M_PI;
		brdf = albedo;

		pdf = dot(uvw.w(), scattered.direction())/M_PI;

		return true;
	}

	std::string type = "lambertian";

	vec3 albedo;
};

#endif // !LAMBERT_H
