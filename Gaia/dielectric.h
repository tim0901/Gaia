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
	dielectric(float ref_ind, vec3 a) :ref_indx(ref_ind), albedo(a) {}

	float scattering_pdf(const ray &incident, const hit_record &rec, const ray &scattered) const {
		return 0;
	}
	
	virtual bool scatter(const ray &incident, const hit_record &rec, vec3 &alb, ray &scattered, float &pdf) const {
		vec3 outward_normal;
		vec3 reflected = reflect(incident.direction(), rec.normal);
		float ni_over_nt;
		alb = albedo;
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
			scattered = ray(rec.p, reflected);
		}
		else {
			scattered = ray(rec.p, refracted);
		}

		return true;
	}

	std::string type = "dielectric";

	vec3 albedo;
	float ref_indx;
};

float schlick_approximation(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}

#endif // !DIELECTRIC_H
