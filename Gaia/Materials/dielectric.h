#pragma once

#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "material.h"
#include "random.h"

//A perfect dielectric material

float schlick_approximation(float cosine, float ref_idx);

//Describes a perfectly refractive glass material
class dielectric :public material {
public:
	dielectric(float ref_ind, const vec3& a) :ref_indx(ref_ind), albedo(new solid_colour(a)) {}
	dielectric(float ref_ind, texture* a) :ref_indx(ref_ind), albedo(a) {}
	~dielectric() { delete albedo; }
	virtual bool scatter(const ray &incident, const hit_record &rec, scattering_record &scatter) const {
		vec3 outward_normal;
		vec3 reflected = reflect(incident.direction(), rec.normal);
		float ni_over_nt;
		scatter.brdf = albedo->value(vec2(rec.u, rec.v), rec.p);
		scatter.is_specular = true;
		scatter.pdf = 0;
		vec3 refracted;
		float reflection_prob;
		float cosine;

		if (dot(incident.direction(), rec.normal) > 0) {
			//set outward normal as the negative of the normal of the surface being hit
			outward_normal = -rec.normal;
			ni_over_nt = ref_indx;
			cosine = ref_indx * dot(incident.direction(), rec.normal) / incident.direction().length();
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_indx;
			cosine = -dot(incident.direction(), rec.normal) / incident.direction().length();
		}

		if (refract(incident.direction(), outward_normal, ni_over_nt, refracted)) {
			//refract
			reflection_prob = schlick_approximation(cosine, ref_indx);
		}
		else {
			//reflect
			reflection_prob = 1.0;
		}

		//Randomly assign rays to reflection and refraction, depending on their reflection probabilities
		if (drand48() < reflection_prob) {
			scatter.specular_ray = ray(rec.p, reflected);
		}
		else {
			scatter.specular_ray = ray(rec.p, refracted);
		}

		return true;
	}

	std::string type = "dielectric";

	texture* albedo;
	float ref_indx;
};

#endif // !DIELECTRIC_H
