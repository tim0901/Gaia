#pragma once

#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include "vec3.h"
#include "material.h"
#include "random.h"

//An incorrect implementation of the Phong shading model

class phong :public material {
public:
	phong(const vec3& a) :albedo(a) {}
	//Describes how a ray is scattered

	//DOES NOT WORK

	virtual bool scatter(const ray &incident, const hit_record &rec, vec3 &alb, ray &scattered, float &pdf) const {
		float kd, ks, n;

		
		//New ray direction is uniformly sampled from a sphere

		vec3 rand = random_in_unit_sphere();
		vec3 scatter = rec.p + rec.normal + (2 * dot(rec.normal, rand)* rec.normal - rand);
		scattered = ray(rec.p, scatter-rec.p);

		float top = dot(scatter,incident.direction());
		float bottom = dot(rec.normal, rand);

		float brdf = top / bottom;

		alb = albedo;

		return true;

		
	}

	std::string type = "phong";

	vec3 albedo;
};

#endif // !BLINN_PHONG_H
