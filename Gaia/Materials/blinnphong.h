#pragma once

#ifndef BLINN_PHONG_H
#define BLINN_PHONG_H

#include "vec3.h"
#include "material.h"
#include "random.h"

//An incorrect implementation of the Phong shading model

class phong :public material {
public:
	phong(const vec3& a, object* ll, int ll_size, float amb, float diff, float spec, float shin, vec3 cameraPos) :albedo(new solid_colour(a)), light_list(ll), light_list_size(ll_size), amb_coeff(amb), diff_coeff(diff), spec_coeff(spec), shininess(shin), camPos(cameraPos) {}
	phong(texture* a, object* ll, int ll_size, float amb, float diff, float spec, float shin, vec3 cameraPos) :albedo(a), light_list(ll), light_list_size(ll_size), amb_coeff(amb), diff_coeff(diff), spec_coeff(spec), shininess(shin), camPos(cameraPos) {}
	~phong() { delete albedo; }

	float scattering_pdf(const ray& incident, const hit_record& rec, const ray& scattered) const {
		return 0;
	}

	virtual vec3 emitted(const ray& incident, const hit_record& rec, float u, float v, const vec3& p) const {

		return vec3(0, 0, 0);
	}

	//Describes how a ray is scattered
	virtual bool scatter(const ray& incident, const hit_record& rec, scattering_record& scatter) const {

		vec3 ambientLighting(1, 1, 1);

		//For non-white lights. TODO
		vec3 lightSourceSpecularDistribution(1, 1, 1);
		vec3 lightSourceDiffuseDistribution(1, 1, 1);

		//Ambient term
		scatter.brdf = albedo->value(vec2(rec.u, rec.v), rec.p) * amb_coeff * ambientLighting;
		//Diffuse and Specular Terms
		
		//New ray direction is uniformly sampled from a sphere
		//vec3 rand_dir = random_in_unit_sphere();


		//Do this for every light in the scene
		for (int i = 0; i < 1; i++) {
			
			//Randomly sample the light using its pdf.
			object_pdf lightPdf(light_list, rec.p);
			vec3 lightDirection = lightPdf.generate();
			//Create ray from this direction
			lightDirection = vec3(0, 1, 0);

			ray r(rec.p, lightDirection);
			scatter.specular_ray = r;

			//Temporary hit record
			hit_record tempRec;

			//If the ray intersects the light, include it's weighting.
			if (light_list->hit(r, 0, 0, tempRec)) {

				//Calculate diffuse dot product
				float diffuseDotProduct = dot(lightDirection, rec.normal);
				
				//Add diffuse term to brdf
				scatter.brdf += albedo->value(vec2(rec.u, rec.v), rec.p) * (diff_coeff * diffuseDotProduct * lightSourceDiffuseDistribution);

				//Specular term should only be included if dot product in diffuse term is positive
				if (diffuseDotProduct > 0) {
					//Specular ray dot product
					vec3 top = camPos + lightDirection;
					vec3 bottom = top;
					bottom.make_unit_vector();

					float nDotH = dot(rec.normal, top/bottom);

					scatter.brdf += (spec_coeff * (pow(nDotH, shininess) * lightSourceSpecularDistribution));
				}					
				else {
					//scatter.is_specular = false;
					//scatter.pdf = new cosine_pdf(rec.normal);
				}
			}
		}
		scatter.is_specular = true;
		scatter.pdf = 0;
		return true;
	}

	virtual std::string type(int i = 0) const {
		return "phong";
	}

	object* light_list;
	int light_list_size;
	texture* albedo;
	float amb_coeff;
	float spec_coeff;
	float diff_coeff;
	float shininess;
	vec3 camPos;
};

#endif // !BLINN_PHONG_H
