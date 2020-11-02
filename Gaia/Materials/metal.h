#pragma once

#ifndef METAL_H
#define METAL_H

#include "material.h"
#include "random.h"

class metal :public material {
public:
	metal(const vec3& a, float f) :albedo(new solid_colour(a)) { if (f < 1) fuzz = f; else fuzz = 1; }
	metal(texture* a, float f) :albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	//Describes how a ray is scattered
	~metal() { delete albedo; }
	float scattering_pdf(const ray &incident, const hit_record &rec, const ray &scattered) const {
		return 0;
	}
	
	virtual bool scatter(const ray &incident, const hit_record &rec, scattering_record& scatter) const {
		//Ray is reflected perfectly
		vec3 reflected = reflect(unit_vector(incident.direction()), rec.normal);
		
		scatter.is_specular = true;
		scatter.specular_ray = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
		scatter.brdf = albedo->value(vec2(rec.u, rec.v), rec.p);
		scatter.pdf = 0;
		return true;
	}

	std::string type = "metal";

	texture* albedo;
	float fuzz;
};

#endif // !METAL_H
