#pragma once

#ifndef RADIANCE_H
#define RADIANCE_H

#include "ray.h"
#include "object.h"
#include "hit_record.h"

vec3 cast(const ray& r, object *world,int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 radiance;
		vec3 emitted;
		
		//Will reflect up to a depth of 50 times before it gives up

		bool hit = rec.mat_ptr->scatter(r, rec, radiance, scattered);
		emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

		float cos_theta = dot(scattered.direction(), rec.normal);
		vec3 brdf = radiance;

		if (depth < 50 && hit) {
			//Path tracing equation
			return emitted + brdf * cos_theta * cast(scattered, world, depth + 1);
		}
		else {
			//Terminate ray
			return emitted;
		}
	}
	else {
		//Background colour
		return vec3(0, 0, 0);
	}
}

#endif // !RADIANCE_H
