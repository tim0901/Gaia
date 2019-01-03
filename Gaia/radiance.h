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

		if (image->constant_luminance == false) {
			vec3 brdf = radiance;

			if (depth < 50 && hit) {
				//Path tracing equation
				if (image->montecarlo == true) {
					//Use Monte Carlo rendering - doesn't work with reflections
					return emitted + brdf * cos_theta *rec.mat_ptr->scattering_pdf(r, rec, scattered) *cast(image, scattered, world, depth + 1) / pdf;
				}
				else {
					return emitted + brdf * cos_theta * cast(image, scattered, world, depth + 1);
				}

			}
			else {
				//Terminate ray
				return emitted;
			}
		}
		else {
			vec3 brdf = radiance * M_PI;

			if (depth < 50 && hit) {
				//Path tracing equation
				if (image->montecarlo == true) {
					//Use Monte Carlo rendering - doesn't work with reflections
					return brdf * rec.mat_ptr->scattering_pdf(r, rec, scattered) *cast(image, scattered, world, depth + 1) / pdf;
				}
				else {
					return brdf * cast(image, scattered, world, depth + 1);
				}

			}
			else {
				//Terminate ray
				return vec3(0, 0, 0);
			}
		}

	}
	else {
		//Background colour
		return *image->background_colour;
	}
}

#endif // !RADIANCE_H
