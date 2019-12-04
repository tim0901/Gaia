#pragma once

#ifndef OREN_NAYAR_H
#define OREN_NAYAR_H

#include "material.h"
#include "vec3.h"

//Oren-Nayar reflectance model

class oren_nayar : public material {
public:
	oren_nayar(const vec3& a, float r) : albedo(a), roughness(r) {}

	//Unsure if correct
	float scattering_pdf(const ray &incident, const hit_record &rec, const ray &scattered) const {
		float cosine = dot(rec.normal, unit_vector(scattered.direction()));
		if (cosine < 0) {
			cosine = 0;
		}
		return cosine;
	}

	//unsure if pdf is correct
	virtual bool scatter(const ray &incident, const hit_record &rec, scattering_record& scatter) const {
		
		//Generate random scattered ray
		vec3 target; 
		do {
			target = rec.p + rec.normal + random_in_unit_sphere();
		} while (dot(target, rec.normal) < 0);
		scatter.specular_ray = ray(rec.p, unit_vector(target - rec.p));

		float a = 1 - 0.5*((roughness*roughness) / (roughness*roughness + 0.33));
		float b = 0.45*((roughness*roughness) / (roughness*roughness + 0.45));


		vec3 incdir = incident.direction();
		vec3 scadir = scatter.specular_ray.direction();
		float theeta_i = acos(incdir.z() / (incdir.length()));
		float theeta_r = acos(scadir.z() / (scadir.length()));
		float alpha = std::max(theeta_i, theeta_r);
		float phi_i = atan(incdir.x() / incdir.y());
		float phi_r = atan(scadir.x() / scadir.y());;
		float beta = std::min(phi_i, phi_r);
		scatter.is_specular = true;
		scatter.brdf = abs(albedo * (a + (b*std::max(0.f, cos(phi_i - phi_r))*sin(alpha)*tan(beta))) / M_PI);

		scatter.pdf = 0;

		return true;
	}

	std::string type = "oren_nayar";

	vec3 albedo;
	float roughness;

};

#endif // !OREN_NAYAR_H
