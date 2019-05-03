#pragma once

#ifndef SCENES_H
#define SCENES_H

#include "vec3.h"
#include "object.h"
#include "object_list.h"
#include "image_parameters.h"
#include "camera.h"
#include "sphere.h"
#include "plane.h"
#include "stb_image.h"
#include "materials.h"
#include "triangle.h"
#include "mesh.h"
#include "aabb.h"
#include "bvh.h"

void cornell_box(object **world, /*object **light_list, */image_parameters *image, camera **cam) {

	//X goes right -> left
	//Y goes down -> up
	//Z goes front -> back

	image->nx = 500;
	image->ny = 500;
	image->ns = 100;
	image->iterative_mode = false;
	image->z_depth_pass = false;
	image->min_z_depth = 2;
	image->max_z_depth = 3.5;
	image->montecarlo = true;

	//Camera
	vec3 look_from(0.5, 0.5, -2);
	vec3 look_at(0.5, 0.5, 1);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 30;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	int i = 0;
	object **list = new object*[50];

	//Materials
	diffuse_light *light = new diffuse_light(new vec3(10, 10, 10));
	lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
	lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
	lambertian *red = new lambertian(vec3(0.65, 0.05, 0.05));
	lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
	lambertian *racinggreen = new lambertian(vec3(0.0, 66.0 / 255.0, 37.0 / 255.0));
	//Objects

	//Floor
//	list[i++] = new rectangle(i,0,vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), white); 
	list[i++] = new triangle(i, 0, new vec3(0, 0, 0), new vec3(1, 0, 1), new vec3(1, 0, 0), white, new vec3(0, 1, 0), new vec3(0, 1, 0), new vec3(0, 1, 0));
	list[i++] = new triangle(i, 0, new vec3(0, 0, 0), new vec3(0, 0, 1), new vec3(1, 0, 1), white, new vec3(0, 1, 0), new vec3(0, 1, 0), new vec3(0, 1, 0));

	//Ceiling
//	list[i++] = new rectangle(i,0,vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1), white);
	list[i++] = new triangle(i, 0, new vec3(0, 1, 0), new vec3(1, 1, 1), new vec3(0, 1, 1), white, new vec3(0, -1, 0), new vec3(0, -1, 0), new vec3(0, -1, 0));
	list[i++] = new triangle(i, 0, new vec3(0, 1, 0), new vec3(1, 1, 0), new vec3(1, 1, 1), white, new vec3(0, -1, 0), new vec3(0, -1, 0), new vec3(0, -1, 0));

	//Ceiling light
	list[i++] = new rectangle(i, 0, vec3(0.3, 0.98, 0.3), vec3(0.3, 0.98, 0.7), vec3(0.7, 0.98, 0.7), light);

	//Back Wall
//	list[i++] = new rectangle(i,0,vec3(0, 1, 1), vec3(0, 0, 1), vec3(1, 0, 1), white);
	list[i++] = new triangle(i, 0, new vec3(0, 1, 1), new vec3(1, 0, 1), new vec3(0, 0, 1), white, new vec3(0, 0, -1), new vec3(0, 0, -1), new vec3(0, 0, -1));
	list[i++] = new triangle(i, 0, new vec3(1, 0, 1), new vec3(0, 1, 1), new vec3(1, 1, 1), white, new vec3(0, 0, -1), new vec3(0, 0, -1), new vec3(0, 0, -1));

	//Left Wall
//	list[i++] = new rectangle(i,0,vec3(1, 0, 1), vec3(1, 0, 0), vec3(1, 1, 0), red);
	list[i++] = new triangle(i, 0, new vec3(1, 0, 1), new vec3(1, 1, 0), new vec3(1, 0, 0), red, new vec3(-1, 0, 0), new vec3(-1, 0, 0), new vec3(-1, 0, 0));
	list[i++] = new triangle(i, 0, new vec3(1, 0, 1), new vec3(1, 1, 1), new vec3(1, 1, 0), red, new vec3(-1, 0, 0), new vec3(-1, 0, 0), new vec3(-1, 0, 0));

	//Right Wall
//	list[i++] = new rectangle(i,0,vec3(0, 1, 0), vec3(0, 0, 0), vec3(0, 0, 1), green);	
	list[i++] = new triangle(i, 0, new vec3(0, 0, 1), new vec3(0, 0, 0), new vec3(0, 1, 0), green, new vec3(1, 0, 0), new vec3(1, 0, 0), new vec3(1, 0, 0));
	list[i++] = new triangle(i, 0, new vec3(0, 0, 1), new vec3(0, 1, 0), new vec3(0, 1, 1), green, new vec3(1, 0, 0), new vec3(1, 0, 0), new vec3(1, 0, 0));

	*world = new object_list(list, i);
}


void mini(object **world, /*object **light_list, */image_parameters *image, camera **cam) {

	image->edge_line_pass = false;
	image->z_depth_pass = false;
	image->iterative_mode = false;

	image->max_z_depth = 250;
	image->min_z_depth = 0;

	image->ns = 10;
	vec3 *skyblue = new vec3(119.0 / 255.0, 181.0 / 255.0, 254.0 / 255.0);
	vec3 *whitecolour = new vec3(1, 1, 1);
	image->saveHDR = false;
	image->save_name = "mini_colour_test";
	image->background_colour = skyblue;

	//X goes right -> left
	//Y goes down -> up
	//Z goes front -> back

	image->nx = 1000;
	image->ny = 500;

	//Camera
	vec3 look_from(-100, -150, 50);
	vec3 look_at(0, 0, 20);
	vec3 up(0, 0, 1); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 40;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	int i = 0;
	object **list = new object*[50];

	//Materials
	diffuse_light *light = new diffuse_light(new vec3(15, 15, 15));
	lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
	lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
	lambertian *red = new lambertian(vec3(0.65, 0.05, 0.05));
	lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
	metal *redmetal = new metal(vec3(0.65, 0.05, 0.05), 0.3);
	metal *minigreen = new metal(vec3(28.0/255.0, 54.0/255.0, 50.0/255.0), 0);
	lambertian *minigreen2 = new lambertian(vec3(28.0 / 255.0, 54.0 / 255.0, 50.0 / 255.0));
	lambertian *racinggreen = new lambertian(vec3(0.0, 66.0 / 255.0, 37.0 / 255.0));
	lambertian *grey = new lambertian(vec3(0.4, 0.4, 0.4));

	dielectric *glass = new dielectric(1.5, vec3(1, 1, 1));

	//Objects

	mesh *test = new mesh(load_mesh(0, "minicooper.obj", redmetal));
	list[i++] = test;
//	list[i++] = new xy_rect(1, 0, -5000, 5000, -5000, 5000, 0, vec3(0, 0, 1), red);

	list[i++] = new sphere(1, 0, vec3(0, 0, -10000), 10000, grey);
	
	*world = new object_list(list, i);
}


void teapot(object **world, /*object **light_list, */image_parameters *image, camera **cam) {

	image->edge_line_pass = false;

	image->background_colour = new vec3(1, 1, 1);

	//X goes right -> left
	//Y goes down -> up
	//Z goes front -> back

	image->nx = 1000;
	image->ny = 500;

	//Camera
	vec3 look_from(-150, 150, 150);
	vec3 look_at(0, 65, 0);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 50;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	int i = 0;
	object **list = new object*[50];

	//Materials
	diffuse_light *light = new diffuse_light(new vec3(15, 15, 15));
	lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
	lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
	lambertian *red = new lambertian(vec3(0.65, 0.05, 0.05));
	lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
	material *redmetal = new metal(vec3(0.65, 0.05, 0.05), 0.5);

	//Objects

	mesh *test = new mesh(load_mesh(i + 1, "rolling_Teapot.obj", redmetal));
	object **meshlist = new object*[1];
	meshlist[0] = test;
	list[i++] = new bvh_node(meshlist, 1, 0, 1);

	*world = new object_list(list, i);
}



//Stores various scenes for testing
void three_spheres(object **world, /*object **light_list, */image_parameters *image, camera **cam) {

	//Set image size
	image->nx = 1000;
	image->ny = 500;

	image->ns = 1000;

	image->z_depth_pass = false;
	image->iterative_mode = false;
	image->montecarlo = false;

	//Camera
	vec3 look_from(0, 0, 2);
	vec3 look_at(0, 0, -1);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 40;
	float aperture = 0.1;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	int i = 0;
	object **list = new object*[50];

	//Materials
	oren_nayar *test = new oren_nayar(vec3(0.5, 0.2, 0.5), 0.1);
	lambertian *test2 = new lambertian(vec3(0.1, 0.2, 0.5));
	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
	metal *met = new metal(vec3(0.8, 0.6, 0.2), 0.1);
	dielectric *glass = new dielectric(1.5, vec3(1, 1, 1));
	diffuse_light *light = new diffuse_light(new vec3(15, 15, 15));


	//Objects
	list[i++] = new sphere(i, 0, vec3(0, 0, -1), 0.5, test);
	list[i++] = new sphere(i, 0, vec3(0, 0, 3), 0.5, yellow);
	list[i++] = new sphere(i, 0, vec3(0, -100.5, -1), 100, yellow);
	list[i++] = new sphere(i, 0, vec3(1, 0, -1), 0.5, met);
	list[i++] = new sphere(i, 0, vec3(-1, 0, -1), 0.5, glass);
	//	list[i++] = new sphere(i, 0, vec3(-1, 0, -1), -0.499, glass);

	list[i++] = new sphere(i, 0, vec3(0, 2, 0), 0.5, light);
	list[i++] = new sphere(i, 0, vec3(-1, 2, 0), 0.5, light);
	list[i++] = new sphere(i, 0, vec3(1, 2, 0), 0.5, light);

	*world = new object_list(list, i);
	/*
	int j = 0;
	hitable **a = new hitable*[2];
	a[j++] = new xz_rect(3, 5, 1, 3, -2, 0);
	*light_list = new hitable_list(a, j);
	*/
}

//Stores various scenes for testing
void goochtest(object **world, /*object **light_list, */image_parameters *image, camera **cam) {

	image->montecarlo = true;
	image->background_colour = new vec3(1, 1, 1);

	//Set image size
	image->nx = 1000;
	image->ny = 500;

	//Camera
	vec3 look_from(1, 0.5, 1);
	vec3 look_at(0, 0.5, 0.5);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = 10;//(look_from - look_at).length();
	int fov = 20;
	float aperture = 0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	int i = 0;
	object **list = new object*[50];

	//Materials
	oren_nayar *test = new oren_nayar(vec3(0.5, 0.2, 0.5), 0.1);
	lambertian *test2 = new lambertian(vec3(0.1, 0.2, 0.5));
	metal *met = new metal(vec3(0.8, 0.6, 0.2), 1);
	dielectric *glass = new dielectric(1.5, vec3(1, 1, 1));
	diffuse_light *light = new diffuse_light(new vec3(25, 25, 25));
	lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
	lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
	lambertian *red = new lambertian(vec3(1, 0, 0));
	lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));

	vec3 *pointat = new vec3(0, -1, 0);
	gooch *goochtest = new gooch(0.4, 0.4, 0.2, 0.6, pointat, red);

	//Objects
	list[i++] = new sphere(i, 0, vec3(0, 0.5, 0.5), 0.05, goochtest);
	list[i++] = new sphere(i, 0, vec3(0, -100.5, -1), 100, white);

	*world = new object_list(list, i);
	/*
	int j = 0;
	hitable **a = new hitable*[2];
	a[j++] = new xz_rect(3, 5, 1, 3, -2, 0);
	*light_list = new hitable_list(a, j);
	*/
}

//Not currently supported
void film(object **world, /*object **light_list, */image_parameters *image, camera **cam, int frameno) {

	//Camera
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
	list[i++] = new sphere(i, 0, vec3(0, 0, -1), 0.5, new lambertian(vec3(r, g, b)));
	list[i++] = new sphere(i, 0, vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0)));
	list[i++] = new sphere(i, 0, vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0));
	list[i++] = new sphere(i, 0, vec3(-1, 0, -1), 0.5, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(i, 0, vec3(-1, 0, -1), -0.499, new dielectric(1.5, vec3(1, 1, 1)));
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

	image->edge_line_pass = false;
	image->saveHDR = false;
	image->save_name = "v 0.1 z depth test 2";
	image->z_depth_pass = false;
	image->montecarlo = false;
	float min_z_depth = 5;
	float max_z_depth = 25;
	image->background_colour = new vec3(1, 1, 1);

	vec3 look_from(13, 2, 3);
	vec3 look_at(0, 0, 0);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float dist_to_focus = 10;// (look_from - look_at).length();
	int fov = 20;
	float aperture = 0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus);

	int i = 0;
	int n = 500;
	object **list = new object*[n + 5];
	object **templist = new object*[n + 5];

	list[i++] = new sphere(i, 0, vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	list[i++] = new sphere(i, 0, vec3(0, 1, 0), 1.0, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(i, 0, vec3(0, 1, 0), -0.999, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(i, 0, vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(i, 0, vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.5));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = drand48();
			vec3 center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					list[i++] = new sphere(i, 0, center, 0.2, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(i, 0, center, 0.2,
						new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())), 0.5));
				}
				else {  // glass
					list[i++] = new sphere(i, 0, center, 0.2, new dielectric(1.5, vec3(1, 1, 1)));
				}
			}
		}
	}
	templist[0] = new bvh_node(list, i, 0, 1);

	*world = new object_list(templist, 1);
	//*world = new object_list(list, i);
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