#pragma once

#ifndef GOOCH_H
#define GOOCH_H

#include "material.h"
#include "random.h"


class gooch :public material {
public:
	gooch(float b, float y, float al, float be, vec3* p, const material* m) : warm(b), cool(y), alpha(al), beta(be), pointat(p) { mat = m; //type = mat->type;
}
    ~gooch(){}
	//B and Y determine the strength of the overall temperature shift
	//α determines the prominence of the object colour 
	//β determines the strength of the luminance shift

	float scattering_pdf(const ray &incident, const hit_record &rec, const ray &scattered) const {
		
		//Use PDF of base material
		return mat->scattering_pdf(incident,rec, scattered);
	}

	virtual bool scatter(const ray &incident, const hit_record &rec, scattering_record &scatter) const {



		//Render base material
		mat->scatter(incident, rec, scatter);

		//Find angle between normal and light direction
		vec3 tolight = unit_vector(*pointat - rec.p);
		float cosine = dot(tolight, rec.normal);

		vec3 kcool = vec3(0, 0, warm) + alpha * scatter.brdf;
		vec3 kwarm = vec3(cool, cool, 0) + beta * scatter.brdf;

		//Tint material
		scatter.brdf = ((1 + cosine) / 2)*kcool + (1 - ((1 + cosine) / 2))*kwarm;
		

		scatter.pdf = scatter.pdf; //keep pdf of base material

		return true;
	}

	virtual std::string type(int i = 0) const {
		return mat->type();
	}

	float cool, warm, alpha, beta;
	vec3 *pointat;
	const material *mat;
};

#endif //GOOCH_H
