#pragma once

#ifndef METAL_H
#define METAL_H

#include "material.h"
#include "random.h"

class metal :public material {
public:
	metal(const vec3& a, float f) :albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	//Describes how a ray is scattered

	float scattering_pdf(const ray &incident, const hit_record &rec, const ray &scattered) const {
		return 0;
	}
	
	virtual bool scatter(const ray &incident, const hit_record &rec, vec3 &brdf, ray &scattered, float &pdf) const {
		//Ray is reflected perfectly
		vec3 reflected = reflect(unit_vector(incident.direction()), rec.normal);
		//Fires new ray
		scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());

		float cos_theta = dot(scattered.direction(), rec.normal);

		//Absorbs a little of the colour of the material
		brdf = cos_theta * albedo;
		return (dot(scattered.direction(),rec.normal)>0);
	}

	std::string type = "metal";

	vec3 albedo;
	float fuzz;
};

#endif // !METAL_H
