#pragma once

#ifndef SCENES_H
#define SCENES_H

#include "vec3.h"
#include "object.h"
#include "object_list.h"
#include "image_parameters.h"
#include "camera.h"
#include "sphere.h"
#include "stb_image.h"
#include "materials.h"
#include "triangle.h"

//Stores various scenes for testing
void three_spheres(object **world, /*object **light_list, */image_parameters *image, camera **cam) {
	vec3 look_from(0, 0, 2);
	vec3 look_at(0, 0, -1);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 40;
	float aperture = 0.1;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	int i = 0;
	object **list = new object*[5];
	list[i++] = new sphere(vec3(0, 0, -1), 0.5, new oren_nayar(vec3(0.1, 0.2, 0.5), 0.1));
	//	list[i++] = new sphere(vec3(0, 0, -1), 0.5,new lambertian(vec3(0.1,0.2,0.5)));
	list[i++] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0)));
	list[i++] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.1));
	list[i++] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(vec3(-1, 0, -1), -0.499, new dielectric(1.5, vec3(1, 1, 1)));

	list[i++] = new sphere(vec3(0, 1, 0), 0.5, new diffuse_light(new vec3(50, 50, 50)));

	//	list[i++] = new triangle(new vec3(0, 1.5, -1), new vec3(-1, 1.5, -2), new vec3( 0, 1.5, -2), new diffuse_light(new vec3(50, 50, 50)), new vec3(0, 0, 0), new vec3(0, 0, 0), new vec3(0, 0, 0));
	*world = new object_list(list, i);
	/*
	int i = 0;
	texture *pertext = new noise_texture(4);
	list[i++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[i++] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	//list[i++] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[i++] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));

	int j = 0;
	hitable **a = new hitable*[2];
	a[j++] = new xz_rect(3, 5, 1, 3, -2, 0);
	*light_list = new hitable_list(a, j);*/
}

void film(object **world, /*object **light_list, */image_parameters *image, camera **cam, int frameno) {
	vec3 look_from(0, 0, 2);
	vec3 look_at(0, 0, -1);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 40;
	float aperture = 0.1;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);
	float r, g, b;
	if (frameno < 30 || frameno > 59) {
		r = sin((M_PI*frameno / 90) + (M_PI / 2));
	}
	else {
		r = 0;
	}
	if (frameno < 60) {
		g = sin(M_PI*frameno / 90);
	}
	else {
		g = 0;
	}
	if (frameno > 29) {
		b = sin((M_PI*frameno / 90) - (M_PI / 2));
	}
	else {
		b = 0;
	}
	std::cout << r << g << b << std::endl;
	int i = 0;
	object **list = new object*[5];
	list[i++] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(r, g, b)));
	list[i++] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0)));
	list[i++] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0));
	list[i++] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(vec3(-1, 0, -1), -0.499, new dielectric(1.5, vec3(1, 1, 1)));
	*world = new object_list(list, i);
	/*
	int i = 0;
	texture *pertext = new noise_texture(4);
	list[i++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[i++] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	//list[i++] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[i++] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));

	int j = 0;
	hitable **a = new hitable*[2];
	a[j++] = new xz_rect(3, 5, 1, 3, -2, 0);
	*light_list = new hitable_list(a, j);*/
}


void random_scene(object **world, /*object **light_list, */image_parameters *image, camera **cam) {
	vec3 look_from(13, 2, 3);
	vec3 look_at(0, 0, 0);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float dist_to_focus = 10;// (look_from - look_at).length();
	int fov = 40;
	float aperture = 0.1;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus);

	int i = 0;
	int n = 500;
	object **list = new object*[n + 5];
	list[i++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = drand48();
			vec3 center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(center, 0.2,
						new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())), 0.5));
				}
				else {  // glass
					list[i++] = new sphere(center, 0.2, new dielectric(1.5, vec3(1, 1, 1)));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.5));
	*world = new object_list(list, i);
	/*
	int i = 0;
	texture *pertext = new noise_texture(4);
	list[i++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[i++] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	//list[i++] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[i++] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));

	int j = 0;
	hitable **a = new hitable*[2];
	a[j++] = new xz_rect(3, 5, 1, 3, -2, 0);
	*light_list = new hitable_list(a, j);*/
}

#endif // !RANDOM_SCENE_H