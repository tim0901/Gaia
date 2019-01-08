#pragma once

#ifndef RADIANCE_H
#define RADIANCE_H

#include "ray.h"
#include "object.h"
#include "hit_record.h"

vec3 cast(image_parameters *image, const ray& r, object *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 radiance;
		vec3 emitted;
		float pdf;
		//Will reflect up to a depth of 50 times before it gives up

		bool hit = rec.mat_ptr->scatter(r, rec, radiance, scattered, pdf);
		emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

		float cos_theta = dot(scattered.direction(), rec.normal);

		if (depth < 50 && hit) {
			//Path tracing equation
			if (image->montecarlo == true) {

				//Use Monte Carlo rendering - doesn't work with reflections
				if (image->constant_luminance == false) {
					vec3 brdf = radiance;
					return emitted + brdf * cos_theta *rec.mat_ptr->scattering_pdf(r, rec, scattered) *cast(image, scattered, world, depth + 1) / pdf;
				}
				else {
					vec3 brdf = radiance * M_PI;
					return brdf * rec.mat_ptr->scattering_pdf(r, rec, scattered) *cast(image, scattered, world, depth + 1) / pdf;
				}
			}
			else {
				if (image->constant_luminance == false) {
					vec3 brdf = radiance;
					return emitted + brdf * cos_theta * cast(image, scattered, world, depth + 1);
				}
				else {
					vec3 brdf = radiance * M_PI;
					return brdf * cast(image, scattered, world, depth + 1);
				}

			}

		}
		else {
			//Terminate ray
			return emitted;
		}


	}
	else {
		//Background colour
		//return *image->background_colour;
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y() + 1.0);
		return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);

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