#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "pi.h"
#include "vec3.h"
#include "ray.h"
#include "random.h"

class camera {
public:

	//A simple camera class, to be replaced later
	camera() {}
	camera(vec3 look_from, vec3 look_at, vec3 up, float fov, float aspect_ratio, float aperture, float focal_length){

		float theta = fov * M_PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect_ratio * half_height;

		//Radius of the lens for depth of field
		lens_radius = aperture / 2;

		//Creates basis vectors for the camera
		w = unit_vector(look_from - look_at); //Direction camera points in
		u = unit_vector(cross(up, w));
		v = cross(w, u);

		origin = look_from;
		lower_left = origin - half_width * focal_length * u - half_height * focal_length * v - focal_length * w;
		horizontal = 2 * half_width * focal_length * u;
		vertical = 2 * half_height * focal_length * v;
	}

	//Returns a ray at a random angle taken from a uniformly sampled disk of size equal to the lens
	ray get_ray(float s, float t) {
		vec3 ray_direction = lens_radius * random_in_unit_disk();
		vec3 offset = u * ray_direction.x() + v * ray_direction.y();
		return ray(origin + offset, lower_left + s * horizontal + t * vertical - origin - offset);
	}

	vec3 u, v, w;
	vec3 vertical;
	vec3 lower_left;
	vec3 origin;
	vec3 horizontal;
	float lens_radius;

};
#endif // !CAMERA_H
