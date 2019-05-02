#pragma once

#ifndef RADIANCE_H
#define RADIANCE_H

#include "ray.h"
#include "object.h"
#include "hit_record.h"

vec4 cast(image_parameters *image, const ray& r, object *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 radiance;
		vec3 emitted;
		float pdf;
		//Will reflect up to a depth of 50 times before it gives up

		bool hit = rec.mat_ptr->scatter(r, rec, radiance, scattered, pdf);
		emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

		vec4 emitteda = vec4(emitted.r(), emitted.g(), emitted.b(), 1);
		vec4 radiancea = vec4(radiance.r(), radiance.g(), radiance.b(), 1);

		if (depth < 50 && hit) {
			//Path tracing equation
			vec4 brdf = radiancea;
			if (image->montecarlo == true) {
				//Use Monte Carlo rendering - doesn't work with reflections
				vec4 temp = emitteda + brdf *rec.mat_ptr->scattering_pdf(r, rec, scattered) *cast(image, scattered, world, depth + 1) / pdf;
				return vec4(temp.r(), temp.g(), temp.b(), 1);
			}
			else {
				vec4 temp = emitteda + brdf * cast(image, scattered, world, depth + 1);
				return vec4(temp.r(), temp.g(), temp.b(), 1);
			}

		}
		else {
			//Terminate ray
			return emitteda;
		}


	}
	else {
		//Background colour
		return vec4(image->background_colour->r(), image->background_colour->g(), image->background_colour->b(), 0);
		/*vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
		*/
	}
}

hit_record edge_cast(image_parameters *image, const ray& r, object *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		/*
		ray scattered;
		vec3 radiance;
		vec3 emitted;
		float pdf;
		//Will reflect up to a depth of 50 times before it gives up

		bool hit = rec.mat_ptr->scatter(r, rec, radiance, scattered, pdf);
		
		if (depth < 1 && hit) {

			//Deeper rays!
			return rec;// edge_cast(image, scattered, world, depth + 1);

		}
		else {
			//Terminate ray
			return rec;
		}
		*/
		return rec;

	}
	else {
		//Background colour
		return rec;
	}
}

#endif // !RADIANCE_H