#pragma once

#ifndef SCENES_H
#define SCENES_H

#include "vec3.h"
#include "objects.h"
#include "image_parameters.h"
#include "camera.h"

#include "texture.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "stb_image.h"
#pragma clang diagnostic pop

#include "materials.h"
#include "translate.h"
#include "rotate.h"

void cornell_box(object **world, object **light_list, material** matList, image_parameters *image, camera **cam) {

    
	//X goes right -> left
	//Y goes down -> up
	//Z goes front -> back

	image->nx = 1000;
	image->ny = 1000;
	image->ns = 25;
    
    image->chunk_size = 25;
    
	image->iterative_mode = false;
	image->z_depth_pass = false;
	image->edge_line_pass = false;

	image->min_z_depth = 2;
	image->max_z_depth = 3.5;
	image->stratify_divisions = 1;

	image->maxlevel = 0;

	image->saveHDR = false;
	image->savePPM = false;
    image->generateHeatMap = false;
    image->save_name = "sqaretest";

	//Camera
	vec3 look_from(0.5, 0.5, -2);
	vec3 look_at(0.5, 0.5, 1);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 30;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	image->background_colour = new vec3(0, 0, 0);

	//Iterators
	int i = 0;
	int oid = 0;
	int matNo = 0;
	object** list = new object* [50];

	//Materials
	diffuse_light* light = new diffuse_light(new vec3(10, 10, 10));
	matList[matNo++] = light;
	lambertian* white = new lambertian(vec3(0.73, 0.73, 0.73));
	matList[matNo++] = white;
	lambertian* green = new lambertian(vec3(0.12, 0.45, 0.15));
	matList[matNo++] = green;
	lambertian* red = new lambertian(vec3(0.65, 0.05, 0.05));
	matList[matNo++] = red;
	//lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
	//lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
	lambertian* black = new lambertian(vec3(0.05, 0.05, 0.05));
	matList[matNo++] = black;
	//lambertian *racinggreen = new lambertian(vec3(0.0, 66.0 / 255.0, 37.0 / 255.0));
	//metal *redmetal = new metal(vec3(0.65, 0.05, 0.05), 0.3);
	metal* whitemetal = new metal(vec3(0.65, 0.65, 0.65), 0);
	matList[matNo++] = whitemetal;
	//metal *purplemetal = new metal(vec3(28.0 / 255.0, 4.0 / 255.0, 50.0 / 255.0), 0);
	metal* roughmetal = new metal(vec3(0.65, 0.65, 0.65), 0.9);
	matList[matNo++] = roughmetal;

	vec3* pointat = new vec3(0.5, 0.9999, 0.5);
	//    gooch* goochtest = new gooch(0.4, 0.4, 0.2, 0.6, pointat, white);
	gooch* metalgooch = new gooch(0.4, 0.4, 0.2, 0.6, pointat, roughmetal);
	matList[matNo++] = metalgooch;

	metal* minigreen3 = new metal(vec3(6.0 / 255.0, 33.0 / 255.0, 10.0 / 255.0), 0.3);
	matList[matNo++] = minigreen3;
	dielectric* glass = new dielectric(1.5, vec3(1.0, 1.0, 1.0));
	matList[matNo++] = glass;
	metal* chrome = new metal(vec3(0.4, 0.4, 0.4), 0.9);
	matList[matNo++] = chrome;
	dielectric* redglass = new dielectric(1.5, vec3(0.65, 0.05, 0.05));
	matList[matNo++] = redglass;
	lambertian* beige = new lambertian(vec3(207.0 / 255.0, 185.0 / 255.0, 151.0 / 255.0));
	matList[matNo++] = beige;

	//Light List. Remember to remove objects from Light list, can cause issues when rendering lambertians 
	int j = 0;
	object** llist = new object* [50];

	// Main light
	llist[j++] = new xz_rect(i, 0, 0.3, 0.7, 0.3, 0.7, 0.999, vec3(0, -1, 0), 0);

	// These are the main light, but duplicated to account for portal effects
	llist[j++] = new xz_rect(i, 0, 1.3, 1.7, 0.3, 0.7, 0.999, vec3(0, -1, 0), 0);
	llist[j++] = new xz_rect(i, 0, 0.3, 0.7, 1.3, 1.7, 0.999, vec3(0, -1, 0), 0);

	//llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	//llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));

	// These are the main light, but duplicated to account for portal effects
	//llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 1.3), vec3(0.7, 0.999, 1.7), vec3(0.3, 0.999, 1.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	//llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 1.3), vec3(0.7, 0.999, 1.3), vec3(0.7, 0.999, 1.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	//llist[j++] = new triangle(oid++, 0, vec3(1.3, 0.999, 0.3), vec3(1.7, 0.999, 0.7), vec3(1.3, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	//llist[j++] = new triangle(oid++, 0, vec3(1.3, 0.999, 0.3), vec3(1.7, 0.999, 0.3), vec3(1.7, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));

	//llist[j++] = new sphere(oid++, 0, vec3(0.3, 0.2, 0.5), 0.2, 0);
	//llist[j++] = new xy_rect(i++, 0, 0.3, 0.7, 0.3, 0.7, 0.99999, vec3(0, 0, 1), 0);
	//llist[j++] = new rotate_y(new box(i++, vec3(0.6, 0, 0.6), vec3(0.8, 0.2, 0.8), 0), 10);

	
	// Metal cube
	vec3 p0 = vec3(0.6, 0, 0.7);
	vec3 p1 = vec3(0.8, 0.2, 0.9);
	
	llist[j++] = new translate(new rotate_y(new xy_rect(oid++, 0, p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), vec3(0, 0, -1), 0), -15), vec3(0.3, 0, -0.25));
	llist[j++] = new translate(new rotate_y(new xy_rect(oid++, 0, p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), vec3(0, 0, 1), 0), -15), vec3(0.3, 0, -0.25));

	llist[j++] = new translate(new rotate_y(new xz_rect(oid++, 0, p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), vec3(0, 1, 0), 0), -15), vec3(0.3, 0, -0.25));
	llist[j++] = new translate(new rotate_y(new xz_rect(oid++, 0, p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), vec3(0, -1, 0), 0), -15), vec3(0.3, 0, -0.25));

	llist[j++] = new translate(new rotate_y(new yz_rect(oid++, 0, p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), vec3(1, 0, 0), 0), -15), vec3(0.3, 0, -0.25));
	llist[j++] = new translate(new rotate_y(new yz_rect(oid++, 0, p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), vec3(-1, 0, 0), 0), -15), vec3(0.3, 0, -0.25));
	

		//Assign light list
	*light_list = new object_list(llist, j);

	//Object list
	
	//Floor
	list[i++] = new xz_rect(oid++, 0, 0, 1, 0, 1, 0, vec3(0, 1, 0), white);
	//list[i++] = new rectangle(oid++,0,vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), white);
	//list[i++] = new triangle(oid++, 0, vec3(0, 0, 0), vec3(1, 0, 1), vec3(1, 0, 0), white, vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0));
	//list[i++] = new triangle(oid++, 0, vec3(0, 0, 0), vec3(0, 0, 1), vec3(1, 0, 1), white, vec3(0, 1, 0), vec3(0, 1, 0), vec3(0, 1, 0));

	//Ceiling
	list[i++] = new xz_rect(oid++, 0, 0, 1, 0, 1, 1, vec3(0, -1, 0), white);
	//list[i++] = new rectangle(oid++,0,vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1), white);
	//list[i++] = new triangle(oid++, 0, vec3(0, 1, 0), vec3(1, 1, 1), vec3(0, 1, 1), white, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	//list[i++] = new triangle(oid++, 0, vec3(0, 1, 0), vec3(1, 1, 0), vec3(1, 1, 1), white, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));

	//Ceiling light
	list[i++] = new xz_rect(oid++, 0, 0.3, 0.7,0.3, 0.7, 0.999, vec3(0,-1,0), light);
	//list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	//list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));

	//Back Wall
	list[i++] = new xy_rect(oid++, 0, 0, 1, 0, 1, 1, vec3(0, 0, -1), white);
	//list[i++] = new rectangle(oid++,0,vec3(0, 1, 1), vec3(0, 0, 1), vec3(1, 0, 1), white);
	//list[i++] = new triangle(oid++, 0, vec3(0, 1, 1), vec3(1, 0, 1), vec3(0, 0, 1), white, vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1));
	//list[i++] = new triangle(oid++, 0, vec3(1, 0, 1), vec3(0, 1, 1), vec3(1, 1, 1), white, vec3(0, 0, -1), vec3(0, 0, -1), vec3(0, 0, -1));

	//Left Wall
	list[i++] = new yz_rect(oid++, 0, 0, 1, 0, 1, 1, vec3(-1, 0, 0), red); 
	//list[i++] = new rectangle(oid++,0,vec3(1, 0, 1), vec3(1, 0, 0), vec3(1, 1, 0), red);
	//list[i++] = new triangle(oid++, 0, vec3(1, 0, 1), vec3(1, 1, 0), vec3(1, 0, 0), red, vec3(-1, 0, 0), vec3(-1, 0, 0), vec3(-1, 0, 0));
	//list[i++] = new triangle(oid++, 0, vec3(1, 0, 1), vec3(1, 1, 1), vec3(1, 1, 0), red, vec3(-1, 0, 0), vec3(-1, 0, 0), vec3(-1, 0, 0));

	//Right Wall
	list[i++] = new yz_rect(oid++, 0, 0, 1, 0, 1, 0, vec3(1, 0, 0), green);
	//list[i++] = new rectangle(oid++,0,vec3(0, 1, 0), vec3(0, 0, 0), vec3(0, 0, 1), green);
	//list[i++] = new triangle(oid++, 0, vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0), green, vec3(1, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0));
	//list[i++] = new triangle(oid++, 0, vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1), green, vec3(1, 0, 0), vec3(1, 0, 0), vec3(1, 0, 0));
	
    //Glass Sphere
	//list[i++] = new translate(new sphere(oid++, 0, vec3(0.3, 0.2, 0.5), 0.2, glass),vec3(0,0,0));
	//list[i++] = new translate(new sphere(oid++, 0, vec3(0.3, 0.2, 0.5), -0.199, glass),vec3(0,0,0));

    
    //Metal cube
	list[i++] = new translate(new rotate_y(new box(oid++, vec3(0.6, 0, 0.7), vec3(0.8, 0.2, 0.9), whitemetal), -15), vec3(0.3, 0, -0.25));
	
	//Mini cooper mesh materials
	material** miniMatList = new material* [50];
	miniMatList[0] = chrome;
	miniMatList[1] = black;
	miniMatList[2] = black;
	miniMatList[3] = beige;
	miniMatList[4] = minigreen3;
	miniMatList[5] = glass;
    miniMatList[6] = redglass;

    material** bunnyMatList = new material* [5];
	bunnyMatList[0] = metalgooch;

	
	//Load raw mesh from file
	raw_mesh raw_bunny = load_mesh(oid++, image, "Render Assets/stanfordbunny.obj", bunnyMatList);

	//Initialize mesh
	mesh* bunnymesh = new mesh(&raw_bunny, 0, 0.8);

	//Add mesh to scene
	//list[i++] = new translate( new rotate_y(bunnymesh, -120), vec3(0.7, 0.075, 0.2));
	list[i++] = new translate( new rotate_y(bunnymesh, -120), vec3(0.1, 0.2, 0.5));

	
    //Load raw mesh from file
    //raw_mesh rawMini = load_mesh(oid++, image, "Render Assets/minitest.obj", miniMatList);

    //mesh* miniMesh = new mesh(&rawMini, 0, 0.0025);
    
	//mesh* miniMesh2 = new mesh(&rawMini, 0, 0.001);
    
	//list[i++] = new translate( new rotate_y(new rotate_z(new rotate_y(miniMesh, 90), 90),145), vec3(0.7, 0.075, 0.35));
    
    //list[i++] = new translate( new rotate_y(new rotate_z(new rotate_y(miniMesh2, 90), 90),55), vec3(0.3, 0.2, 0.5));

	//list[i++] = new sphere(oid++, 0, vec3(0.8, 0.1, 0.8), 0.1, whitemetal);

	//list[i++] = new ellipse(oid++, 0, vec3(0.3, 0.2, 0.5), vec3(0.2, 0, 0.1), vec3(0, 0.15, -0.3), red);
	
	
	
	portal* portal1 = new portal(new xy_rect(oid++, 0, 0.4, 0.6, 0.1, 0.3, 0.995, vec3(0, 0, -1), 0), new yz_rect(oid++, 0, 0.1, 0.3, 0.4, 0.6, 0.995, vec3(1, 0, 0), 0));
	matList[matNo++] = portal1;
	portal* portal2 = new portal(new yz_rect(oid++, 0, 0.1, 0.3, 0.4, 0.6, 0.995, vec3(1, 0, 0), 0), new xy_rect(oid++, 0, 0.4, 0.6, 0.1, 0.3, 0.995, vec3(0, 0, -1), 0));
	matList[matNo++] = portal2;

	xy_rect* backWall = new xy_rect(oid++, 0, 0.4, 0.6, 0.1, 0.3, 0.995, vec3(0, 0, -1), portal1);
	yz_rect* leftWall = new yz_rect(oid++, 0, 0.1, 0.3, 0.4, 0.6, 0.995, vec3(-1, 0, 0), portal2);
	list[i++] = backWall;
	list[i++] = leftWall;
	

	lambertian* earthmap = new lambertian(new image_texture("Render Assets/earthmap.jpg"));
	matList[matNo++] = earthmap;
	list[i++] = new xy_rect(oid++, 0, 0.25, 0.75, 0.55, 0.95, 0.999, vec3(0, 0, -1), earthmap);
	//list[i++] = new sphere(oid++, 0, vec3(0.5, 0.2, 0.5), 0.2, earthmap);


	/*
	ellipse* backWall = new ellipse(oid++, 0, vec3(0.5, 0.25, 0.995), vec3(0.1, 0, 0), vec3(0, 0.2, 0), 0, vec3(0, -1, 0));
	ellipse* leftWall = new ellipse(oid++, 0, vec3(0.995, 0.25, 0.5), vec3(0, 0, 0.1), vec3(0, 0.2, 0), 0, vec3(-1, 0, 0));

	portal* portal1 = new portal(backWall, leftWall);
	matList[matNo++] = portal1;
	portal* portal2 = new portal(leftWall, backWall);
	matList[matNo++] = portal2;

	backWall->mat_ptr = portal1;
	leftWall->mat_ptr = portal2;
	list[i++] = leftWall;
	list[i++] = backWall;
	*/
    //Assign world list
	*world = new object_list(list, i);
}

void big_cornell_box(object** world, object** light_list, material** matList, image_parameters* image, camera** cam) {


	//X goes right -> left
	//Y goes down -> up
	//Z goes front -> back

	image->nx = 1000;
	image->ny = 1000;
	image->ns = 200;

	image->chunk_size = 25;

	image->iterative_mode = false;
	image->z_depth_pass = false;
	image->edge_line_pass = false;

	image->min_z_depth = 2;
	image->max_z_depth = 3.5;
	image->stratify_divisions = 1;

	image->maxlevel = 0;

	image->saveHDR = false;
	image->savePPM = false;
	image->generateHeatMap = false;
	image->save_name = "sqaretest";

	//Camera
	vec3 look_from(0.5 * 256, 0.5 * 256, -2 * 256);
	vec3 look_at(0.5 * 256, 0.5 * 256, 1 * 256);
	vec3 up(0, 1 * 256, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 30;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	image->background_colour = new vec3(0, 0, 0);

	//Iterators
	int i = 0;
	int oid = 0;
	int matNo = 0;
	object** list = new object * [50];

	//Materials
	diffuse_light* light = new diffuse_light(new vec3(10, 10, 10));
	matList[matNo++] = light;
	lambertian* white = new lambertian(vec3(0.73, 0.73, 0.73));
	matList[matNo++] = white;
	lambertian* green = new lambertian(vec3(0.12, 0.45, 0.15));
	matList[matNo++] = green;
	lambertian* red = new lambertian(vec3(0.65, 0.05, 0.05));
	matList[matNo++] = red;
	//lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
	//lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
	lambertian* black = new lambertian(vec3(0.05, 0.05, 0.05));
	matList[matNo++] = black;
	//lambertian *racinggreen = new lambertian(vec3(0.0, 66.0 / 255.0, 37.0 / 255.0));
	//metal *redmetal = new metal(vec3(0.65, 0.05, 0.05), 0.3);
	metal* whitemetal = new metal(vec3(0.65, 0.65, 0.65), 0);
	matList[matNo++] = whitemetal;
	//metal *purplemetal = new metal(vec3(28.0 / 255.0, 4.0 / 255.0, 50.0 / 255.0), 0);
	metal* roughmetal = new metal(vec3(0.65, 0.65, 0.65), 0.9);
	matList[matNo++] = roughmetal;

	vec3* pointat = new vec3(0.5 * 256, 0.9999 * 256, 0.5 * 256);
	//    gooch* goochtest = new gooch(0.4, 0.4, 0.2, 0.6, pointat, white);
	gooch* metalgooch = new gooch(0.4, 0.4, 0.2, 0.6, pointat, roughmetal);
	matList[matNo++] = metalgooch;

	metal* minigreen3 = new metal(vec3(6.0 / 255.0, 33.0 / 255.0, 10.0 / 255.0), 0.3);
	matList[matNo++] = minigreen3;
	dielectric* glass = new dielectric(1.5, vec3(1.0, 1.0, 1.0));
	matList[matNo++] = glass;
	metal* chrome = new metal(vec3(0.4, 0.4, 0.4), 0.9);
	matList[matNo++] = chrome;
	dielectric* redglass = new dielectric(1.5, vec3(0.65, 0.05, 0.05));
	matList[matNo++] = redglass;
	lambertian* beige = new lambertian(vec3(207.0 / 255.0, 185.0 / 255.0, 151.0 / 255.0));
	matList[matNo++] = beige;

	//Light List. Remember to remove objects from Light list, can cause issues when rendering lambertians 
	int j = 0;
	object** llist = new object * [50];

	// Main light
	llist[j++] = new xz_rect(i, 0, 0.3 * 256, 0.7 * 256, 0.3 * 256, 0.7 * 256, 0.999 * 256, vec3(0, -1, 0), 0);

	// These are the main light, but duplicated to account for portal effects
	llist[j++] = new xz_rect(i, 0, 1.3 * 256, 1.7 * 256, 0.3 * 256, 0.7 * 256, 0.999 * 256, vec3(0, -1, 0), 0);
	llist[j++] = new xz_rect(i, 0, 0.3 * 256, 0.7 * 256, 1.3 * 256, 1.7 * 256, 0.999 * 256, vec3(0, -1, 0), 0);

	// Metal cube
	vec3 p0 = vec3(0.6 * 256, 0, 0.7 * 256);
	vec3 p1 = vec3(0.8 * 256, 0.2 * 256, 0.9 * 256);

	llist[j++] = new translate(new rotate_y(new xy_rect(oid++, 0, p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), vec3(0, 0, -1), 0), -15), vec3(0.3 * 256, 0, -0.25 * 256));
	llist[j++] = new translate(new rotate_y(new xy_rect(oid++, 0, p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), vec3(0, 0, 1), 0), -15), vec3(0.3 * 256, 0, -0.25 * 256));

	llist[j++] = new translate(new rotate_y(new xz_rect(oid++, 0, p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), vec3(0, 1, 0), 0), -15), vec3(0.3 * 256, 0, -0.25 * 256));
	llist[j++] = new translate(new rotate_y(new xz_rect(oid++, 0, p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), vec3(0, -1, 0), 0), -15), vec3(0.3 * 256, 0, -0.25 * 256));

	llist[j++] = new translate(new rotate_y(new yz_rect(oid++, 0, p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), vec3(1, 0, 0), 0), -15), vec3(0.3 * 256, 0, -0.25 * 256));
	llist[j++] = new translate(new rotate_y(new yz_rect(oid++, 0, p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), vec3(-1, 0, 0), 0), -15), vec3(0.3 * 256, 0, -0.25 * 256));


	//Assign light list
	*light_list = new object_list(llist, j);

	//Object list

	//Floor
	list[i++] = new xz_rect(oid++, 0, 0, 256, 0, 256, 0, vec3(0, 1, 0), white);
	
	//Ceiling
	list[i++] = new xz_rect(oid++, 0, 0, 256, 0, 256, 256, vec3(0, -1, 0), white);
	
	//Ceiling light
	list[i++] = new xz_rect(oid++, 0, 0.3 * 256, 0.7 * 256, 0.3 * 256, 0.7 * 256, 0.999 * 256, vec3(0, -1, 0), light);
	
	//Back Wall
	list[i++] = new xy_rect(oid++, 0, 0, 256, 0, 256, 256, vec3(0, 0, -1), white);
	
	//Left Wall
	list[i++] = new yz_rect(oid++, 0, 0, 256, 0, 256, 256, vec3(-1, 0, 0), red);
	
	//Right Wall
	list[i++] = new yz_rect(oid++, 0, 0, 256, 0, 256, 0, vec3(1, 0, 0), green);

	//Metal cube
	list[i++] = new translate(new rotate_y(new box(oid++, vec3(0.6 * 256, 0, 0.7 * 256), vec3(0.8 * 256, 0.2 * 256, 0.9 * 256), whitemetal), -15), vec3(0.3 * 256, 0, -0.25 * 256));

	//Mini cooper mesh materials
	material** miniMatList = new material * [50];
	miniMatList[0] = chrome;
	miniMatList[1] = black;
	miniMatList[2] = black;
	miniMatList[3] = beige;
	miniMatList[4] = minigreen3;
	miniMatList[5] = glass;
	miniMatList[6] = redglass;

	material** bunnyMatList = new material * [5];
	bunnyMatList[0] = metalgooch;


	//Load raw mesh from file
	raw_mesh raw_bunny = load_mesh(oid++, image, "Render Assets/stanfordbunny.obj", bunnyMatList);

	//Initialize mesh
	mesh* bunnymesh = new mesh(&raw_bunny, 0, 0.8 * 256);

	//Add mesh to scene
	list[i++] = new translate( new rotate_y(bunnymesh, -120), vec3(0.1* 256, 0.2* 256, 0.5* 256));

	
	ellipse* backWall = new ellipse(oid++, 0, vec3(0.5*256, 0.25 * 256, 0.995 * 256), vec3(0.1 * 256, 0, 0), vec3(0, 0.2 * 256, 0), 0, vec3(0, -1, 0));
	ellipse* leftWall = new ellipse(oid++, 0, vec3(0.995 * 256, 0.25 * 256, 0.5 * 256), vec3(0, 0, 0.1 * 256), vec3(0, 0.2 * 256, 0), 0, vec3(-1, 0, 0));

	portal* portal1 = new portal(backWall, leftWall);
	matList[matNo++] = portal1;
	portal* portal2 = new portal(leftWall, backWall);
	matList[matNo++] = portal2;

	backWall->mat_ptr = portal1;
	leftWall->mat_ptr = portal2;
	list[i++] = leftWall;
	list[i++] = backWall;
	

	lambertian* earthmap = new lambertian(new image_texture("Render Assets/earthmap.jpg"));
	matList[matNo++] = earthmap;
	list[i++] = new xy_rect(oid++, 0, 0.25 * 256, 0.75 * 256, 0.55 * 256, 0.95 * 256, 0.999 * 256, vec3(0, 0, -1), earthmap);

	//Assign world list
	* world = new object_list(list, i);
}


void lucy(object** world, object** light_list, material** matList, image_parameters* image, camera** cam) {


	//X goes right -> left
	//Y goes down -> up
	//Z goes front -> back

	image->nx = 1000;
	image->ny = 1000;
	image->ns = 25;

	image->chunk_size = 25;

	image->iterative_mode = false;
	image->z_depth_pass = false;
	image->edge_line_pass = false;

	image->min_z_depth = 2;
	image->max_z_depth = 3.5;
	image->stratify_divisions = 1;

	image->maxlevel = 0;

	image->saveHDR = false;
	image->savePPM = false;
	image->generateHeatMap = false;
	image->save_name = "lucytest";

	//Camera
	vec3 look_from(0.5, 0.5, -2);
	vec3 look_at(0.5, 0.5, 1);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 30;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	image->background_colour = new vec3(0, 0, 0);

	//Iterators
	int i = 0;
	int oid = 0;
	int matNo = 0;
	object** list = new object * [50];

	//Materials
	diffuse_light* light = new diffuse_light(new vec3(10, 10, 10));
	matList[matNo++] = light;
	lambertian* white = new lambertian(vec3(0.73, 0.73, 0.73));
	matList[matNo++] = white;
	lambertian* green = new lambertian(vec3(0.12, 0.45, 0.15));
	matList[matNo++] = green;
	lambertian* red = new lambertian(vec3(0.65, 0.05, 0.05));
	matList[matNo++] = red;

	//Light List. Remember to remove objects from Light list, can cause issues when rendering lambertians 
	int j = 0;
	object** llist = new object * [50];

	// Main light
	llist[j++] = new xz_rect(i, 0, 0.3, 0.7, 0.3, 0.7, 0.999, vec3(0, -1, 0), 0);

	//Assign light list
	*light_list = new object_list(llist, j);

	//Object list

	//Floor
	list[i++] = new xz_rect(oid++, 0, 0, 1, 0, 1, 0, vec3(0, 1, 0), white);

	//Ceiling
	list[i++] = new xz_rect(oid++, 0, 0, 1, 0, 1, 1, vec3(0, -1, 0), white);

	//Ceiling light
	list[i++] = new xz_rect(oid++, 0, 0.3, 0.7, 0.3, 0.7, 0.999, vec3(0, -1, 0), light);

	//Back Wall
	list[i++] = new xy_rect(oid++, 0, 0, 1, 0, 1, 1, vec3(0, 0, -1), white);

	//Left Wall
	list[i++] = new yz_rect(oid++, 0, 0, 1, 0, 1, 1, vec3(-1, 0, 0), red);

	//Right Wall
	list[i++] = new yz_rect(oid++, 0, 0, 1, 0, 1, 0, vec3(1, 0, 0), green);


	material** lucyMatList = new material * [5];
	lucyMatList[0] = white;

	//Load raw mesh from file
	raw_mesh lucy_raw_mesh = load_mesh(oid++, image, "Render Assets/stanfordbunny.obj", lucyMatList);

	//Initialize mesh
	mesh* lucyMesh = new mesh(&lucy_raw_mesh, 0, 0.0005);

	//Add mesh to scene
	list[i++] = new translate(new rotate_y(lucyMesh, 180), vec3(0.5, 0.393, 0.5));

	//Assign world list
	* world = new object_list(list, i);
}


void bunnyMovie(object **world, object **light_list, image_parameters *image, camera **cam){
    
    image->nx = 1000;
    image->ny = 1000;
    image->ns = 250;
    
    image->chunk_size = 25;
    
    image->iterative_mode = false;
    image->z_depth_pass = false;
    image->edge_line_pass = false;

    image->min_z_depth = 2;
    image->max_z_depth = 3.5;
    image->stratify_divisions = 1;

    image->maxlevel = 3;

    image->saveHDR = true;
    image->savePPM = false;
    image->save_name = "outputTest";
    
    //Camera
    vec3 look_from(0.5, 0.5, -2);
    vec3 look_at(0.5, 0.5, 1);
    vec3 up(0, 1, 0); // vector that is "up" for the camera
    float focal_length = (look_from - look_at).length();
    int fov = 30;
    float aperture = 0.0;
    float aspect_ratio = float(image->nx) / float(image->ny);
    *cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

    image->background_colour = new vec3(0, 0, 0);

    int i = 0;
    int oid = 0;
    object **list = new object*[50];
    
    //Materials
    diffuse_light *light = new diffuse_light(new vec3(10, 10, 10));
//    lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
//    lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
//    lambertian *red = new lambertian(vec3(0.65, 0.05, 0.05));
    //metal *purplemetal = new metal(vec3(28.0 / 255.0, 4.0 / 255.0, 50.0 / 255.0), 0);
    metal* roughmetal = new metal(vec3(0.65,0.65,0.65), 0.9);
    
    vec3 *pointat = new vec3(0.5, 0.9999, 0.5);
//    gooch* goochtest = new gooch(0.4, 0.4, 0.2, 0.6, pointat, white);
    gooch* metalgooch = new gooch(0.4,0.4,0.2,0.6,pointat, roughmetal);
    
//    dielectric* glass = new dielectric(1.5, vec3(1.0, 1.0, 1.0));
    
    
    
    //Object list
    
    //Ceiling light
    list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
    list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));

    //Bunny mesh materials
    material** matList = new material * [5];
    matList[0] = metalgooch;
    
    //Load raw mesh from file
    raw_mesh raw_bunny = load_mesh(oid++, image, "Render Assets/stanfordbunny.obj", matList);

    //Initialize mesh
    mesh* bunnymesh = new mesh(&raw_bunny, 0, 3.0);

    //Add mesh to scene
    list[i++] = new translate( new rotate_y(bunnymesh, -120), vec3(0.5, 0.173, 0.5));

    //Assign world list
    *world = new object_list(list, i);

    //Light List
    int j = 0;
    object** llist = new object* [50];

    llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
    llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
    
    //Assign light list
    *light_list = new object_list(llist, j);
}

void bunny_cornell_box(object **world, object **light_list, material** matList, image_parameters *image, camera **cam) {

    //X goes right -> left
    //Y goes down -> up
    //Z goes front -> back

    image->nx = 1000;
    image->ny = 1000;
    image->ns = 250;
    
    image->chunk_size = 25;
    
    image->iterative_mode = false;
    image->z_depth_pass = false;
    image->edge_line_pass = false;

    image->min_z_depth = 2;
    image->max_z_depth = 3.5;
    image->stratify_divisions = 1;

    image->maxlevel = 0;

    image->saveHDR = true;
    image->savePPM = false;
    image->save_name = "bunnyHeatMapTest";

    //Camera
    vec3 look_from(0.5, 0.5, -2);
    vec3 look_at(0.5, 0.5, 1);
    vec3 up(0, 1, 0); // vector that is "up" for the camera
    float focal_length = (look_from - look_at).length();
    int fov = 30;
    float aperture = 0.0;
    float aspect_ratio = float(image->nx) / float(image->ny);
    *cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

    image->background_colour = new vec3(0, 0, 0);

    int i = 0;
    int oid = 0;
    object **list = new object*[50];


	//Light List
	int j = 0;
	object** llist = new object * [50];

	llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	//llist[j++] = new sphere(oid++, 0, vec3(0.5, 0.5, 0.5), 0.3, 0);
	//Assign light list
	*light_list = new object_list(llist, j);

    //Materials
    diffuse_light *light = new diffuse_light(new vec3(10, 10, 10));
    lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
    lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
    lambertian *red = new lambertian(vec3(0.65, 0.05, 0.05));
    //metal *purplemetal = new metal(vec3(28.0 / 255.0, 4.0 / 255.0, 50.0 / 255.0), 0);
//   metal* roughmetal = new metal(vec3(0.65,0.65,0.65), 0.9);
	oren_nayar* test = new oren_nayar(vec3(0.5, 0.2, 0.5), 0.1);
	phong* phongTest = new phong(vec3(0.65, 0.05, 0.05), *light_list, j, 1.0, 0.0, 0.0, 10, look_from);
    
//    vec3 *pointat = new vec3(0.5, 0.9999, 0.5);
//    gooch* goochtest = new gooch(0.4, 0.4, 0.2, 0.6, pointat, white);
//    gooch* metalgooch = new gooch(0.4,0.4,0.2,0.6,pointat, roughmetal);
    
//    dielectric* glass = new dielectric(1.5, vec3(1.0, 1.0, 1.0));
    
    
    
    //Object list
    
    //Floor
    list[i++] = new rectangle(oid++,0,vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), white);

    //Ceiling
    list[i++] = new rectangle(oid++,0,vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1), white);

    //Ceiling light
    list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
    list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));

    //Back Wall
    list[i++] = new rectangle(oid++,0,vec3(0, 1, 1), vec3(0, 0, 1), vec3(1, 0, 1), white);
    
    //Left Wall
    list[i++] = new rectangle(oid++,0,vec3(1, 0, 1), vec3(1, 0, 0), vec3(1, 1, 0), red);

    //Right Wall
    list[i++] = new rectangle(oid++,0,vec3(0, 1, 0), vec3(0, 0, 0), vec3(0, 0, 1), green);
    
    //Bunny mesh materials
    matList = new material * [5];
    matList[0] = white;
    
    //Load raw mesh from file
    raw_mesh raw_bunny = load_mesh(oid++, image, "Render Assets/stanfordbunny.obj", matList);

    //Initialize mesh
    mesh* bunnymesh = new mesh(&raw_bunny, 0, 3.0);

    //Add mesh to scene
    list[i++] = new translate( new rotate_y(bunnymesh, -120), vec3(0.5, 0.165, 0.5));

	//list[i++] = new sphere(oid++, 0, vec3(0.5, 0.5, 0.5), 0.3, phongTest);

    //Assign world list
    *world = new object_list(list, i);

}

void furnace_test(object **world, object** light_list, image_parameters *image, camera **cam) {

	image->nx = 500;
	image->ny = 500;
	image->ns = 100;
	image->iterative_mode = true;

	image->saveHDR = true;
	image->savePPM = true;
	image->save_name = "furnace test";

	//Camera
	vec3 look_from(0, 0, -2);
	vec3 look_at(0, 0, 1);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 30;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	image->background_colour = new vec3(1, 1, 1);

	int i = 0;
    int oid = 0;
	object **list = new object*[1];

	list[i++] = new sphere(oid++, 0, vec3(0, 0, 0), 0.5, new lambertian(vec3(0.18, 0.18, 0.18)));

	*world = new object_list(list, i);
}

void mini(object** world, object** light_list, material** matList, image_parameters* image, camera** cam) {

	//X goes ?
	//Y goes ?
	//Z goes down -> up

	image->nx = 1000;
	image->ny = 500;
	image->ns = 500;

	image->maxlevel = 2;

	image->edge_line_pass = false;
	image->z_depth_pass = false;
	image->iterative_mode = false;

	image->max_z_depth = 250;
	image->min_z_depth = 0;

//	vec3 *skyblue = new vec3(119.0 / 255.0, 181.0 / 255.0, 254.0 / 255.0);
//	vec3 *whitecolour = new vec3(1, 1, 1);
	image->saveHDR = true;
	image->generateHeatMap = true;
	image->save_name = "miniHeatMap";
	image->background_colour = new vec3(0,0,0);

	//Camera
	vec3 look_from(-100, -150, 50);
	vec3 look_at(0, 0, 20);
	vec3 up(0, 0, 1); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 40;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	//Light List
	int j = 0;
	object** llist = new object * [50];

	llist[j++] = new xy_rect(2, 0, -100, 100, -100, 100, 100, vec3(0, 0, -1), 0);

	*light_list = new object_list(llist, j);
	int i = 0;
	object **list = new object*[50];

	//Materials
	diffuse_light *light = new diffuse_light(new vec3(15, 15, 15));
//	lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
//	lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
//	lambertian *red = new lambertian(vec3(0.65, 0.05, 0.05));
//	lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
	lambertian *black = new lambertian(vec3(0.05, 0.05, 0.05));
//	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
//	metal *redmetal = new metal(vec3(0.65, 0.05, 0.05), 0.3);
//	metal *minigreen = new metal(vec3(28.0/255.0, 54.0/255.0, 50.0/255.0), 0);
//	metal *purplemetal = new metal(vec3(28.0/255.0, 4.0/255.0, 50.0/255.0), 0);
	metal *minigreen3 = new metal(vec3(6.0/255.0, 33.0/255.0, 10.0/255.0), 0.3);
//	lambertian *minigreen2 = new lambertian(vec3(28.0 / 255.0, 54.0 / 255.0, 50.0 / 255.0));
//	lambertian *racinggreen = new lambertian(vec3(0.0, 66.0 / 255.0, 37.0 / 255.0));
	lambertian *grey = new lambertian(vec3(0.4, 0.4, 0.4));
	metal *chrome = new metal(vec3(0.4, 0.4, 0.4),0.9);

	dielectric *glass = new dielectric(1.0, vec3(1, 1, 1));
	dielectric *redglass = new dielectric(1.5, vec3(0.65, 0.05, 0.05));
	lambertian* beige = new lambertian(vec3(207.0 / 255.0, 185.0 / 255.0, 151.0 / 255.0));

	material **miniMatlist = new material*[50];
	miniMatlist[0] = chrome;
	miniMatlist[1] = black;
	miniMatlist[2] = black;
	miniMatlist[3] = beige;
	miniMatlist[4] = minigreen3;
	miniMatlist[5] = glass;
	miniMatlist[6] = redglass;

	//Objects

	raw_mesh raw = load_mesh(0, image, "Render Assets/minitest.obj", miniMatlist);

	mesh *mini_mesh = new mesh(&raw, 0, 1.0);
	list[i++] = new translate(mini_mesh, vec3(0,0,31));
//	list[i++] = new xy_rect(1, 0, -5000, 5000, -5000, 5000, 0, vec3(0, 0, 1), red);

	list[i++] = new sphere(1, 0, vec3(0, 0, -10000), 10000, grey);

	list[i++] = new xy_rect(2, 0, -100, 100, -100, 100, 100, vec3(0, 0, -1), light);

	/*
	list[i++] = new sphere(2, 0, vec3(-100, -150, 300), 150, light);
	list[i++] = new sphere(3, 0, vec3(-100, 0, 300), 100, light);
	list[i++] = new sphere(4, 0, vec3(0, -150, 300), 100, light);
	list[i++] = new sphere(5, 0, vec3(0, 150, 300), 100, light);
	list[i++] = new sphere(6, 0, vec3(100, 0 , 300), 100, light);
	list[i++] = new sphere(7, 0, vec3(100, -150, 300), 100, light);
	list[i++] = new sphere(8, 0, vec3(-100, 150, 300), 100, light);
	list[i++] = new sphere(9, 0, vec3(100, 150, 300), 100, light);
	*/
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
//	diffuse_light *light = new diffuse_light(new vec3(15, 15, 15));
//	lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
//	lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
//	lambertian *red = new lambertian(vec3(0.65, 0.05, 0.05));
	lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
//	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
	material *redmetal = new metal(vec3(0.65, 0.05, 0.05), 0.5);


	material **matlist = new material*[50];
	matlist[0] = redmetal;
	matlist[1] = blue;


	//Objects

//	raw_mesh* raw = &load_mesh(i + 1, image, "Render Assets/rolling_Teapot.obj", matlist);

//	mesh *test = new mesh(raw, new vec3(0,0,0), 1.0);
	object **meshlist = new object*[1];
//	meshlist[0] = test;
	list[i++] = new bvh_node(meshlist, 1, 0, 1);

	*world = new object_list(list, i);
}

void three_spheres(object **world, /*object **light_list, */image_parameters *image, camera **cam) {

	//Set image size
	image->nx = 1000;
	image->ny = 500;

	image->ns = 1000;

	image->z_depth_pass = false;
	image->iterative_mode = false;

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
    int oid = 0;
	object **list = new object*[50];

	//Materials
	oren_nayar *test = new oren_nayar(vec3(0.5, 0.2, 0.5), 0.1);
//	lambertian *test2 = new lambertian(vec3(0.1, 0.2, 0.5));
//	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));
//	metal *met = new metal(vec3(0.8, 0.6, 0.2), 0.1);
//	dielectric *glass = new dielectric(1.5, vec3(1, 1, 1));
	diffuse_light *light = new diffuse_light(new vec3(15, 15, 15));


	//Objects
	list[i++] = new sphere(oid++, 0, vec3(0, 0, -1), 0.5, test);
//	list[i++] = new sphere(oid++, 0, vec3(0, 0, 3), 0.5, yellow);
//	list[i++] = new sphere(oid++, 0, vec3(0, -100.5, -1), 100, yellow);
//	list[i++] = new sphere(oid++, 0, vec3(1, 0, -1), 0.5, met);
//	list[i++] = new sphere(oid++, 0, vec3(-1, 0, -1), 0.5, glass);
	//	list[i++] = new sphere(oid++, 0, vec3(-1, 0, -1), -0.499, glass);

	list[i++] = new sphere(oid++, 0, vec3(0, 2, 0), 0.5, light);
	list[i++] = new sphere(oid++, 0, vec3(-1, 2, 0), 0.5, light);
	list[i++] = new sphere(oid++, 0, vec3(1, 2, 0), 0.5, light);

	*world = new object_list(list, i);
	/*
	int j = 0;
	hitable **a = new hitable*[2];
	a[j++] = new xz_rect(3, 5, 1, 3, -2, 0);
	*light_list = new hitable_list(a, j);
	*/
}

void goochtest(object **world, object **light_list, image_parameters *image, camera **cam) {

	image->background_colour = new vec3(1, 1, 1);

	//Set image size
	image->nx = 1000;
	image->ny = 500;

	//Camera
	vec3 look_from(1, 0.5, 1);
	vec3 look_at(0.4, 0.4, 0.4);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = 10;//(look_from - look_at).length();
	int fov = 20;
	float aperture = 0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

    image->background_colour = new vec3(0,0,0);
    
	int i = 0;
    int oid = 0;
    object **list = new object*[50];

	//Materials
//	oren_nayar *test = new oren_nayar(vec3(0.5, 0.2, 0.5), 0.1);
//	lambertian *test2 = new lambertian(vec3(0.1, 0.2, 0.5));
//	metal *met = new metal(vec3(0.8, 0.6, 0.2), 1);
//	dielectric *glass = new dielectric(1.5, vec3(1, 1, 1));
	diffuse_light *light = new diffuse_light(new vec3(15, 15, 15));
//	lambertian *white = new lambertian(vec3(0.73, 0.73, 0.73));
//	lambertian *green = new lambertian(vec3(0.12, 0.45, 0.15));
	lambertian *red = new lambertian(vec3(1, 0, 0));
//	lambertian *blue = new lambertian(vec3(0.05, 0.05, 0.65));
//	lambertian *yellow = new lambertian(vec3(0.8, 0.8, 0));

	vec3 *pointat = new vec3(0.5, 0.999, 0.5);
	gooch *goochtest = new gooch(0.4, 0.4, 0.2, 0.6, pointat, red);

	//Objects
    //list[i++] = new rotate_y(new box(oid++, vec3(0.3, 0.3, 0.3), vec3(0.4, 0.4, 0.4), goochtest),10);
    
    list[i++] = new sphere(oid++, 0, vec3(0.4,0.4,0.4), 0.1, goochtest);
	//list[i++] = new sphere(oid++, 0, vec3(0, -100.5, -1), 100, white);
    
    //Ceiling light
    list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
    list[i++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), light, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	*world = new object_list(list, i);
    
    
    //Light List
    int j = 0;
    object** llist = new object* [50];

    llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.7), vec3(0.3, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
    llist[j++] = new triangle(oid++, 0, vec3(0.3, 0.999, 0.3), vec3(0.7, 0.999, 0.3), vec3(0.7, 0.999, 0.7), 0, vec3(0, -1, 0), vec3(0, -1, 0), vec3(0, -1, 0));
	
    *light_list = new object_list(llist, j);
    
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
    int oid = 0;
	object **list = new object*[5];
	list[i++] = new sphere(oid++, 0, vec3(0, 0, -1), 0.5, new lambertian(vec3(r, g, b)));
	list[i++] = new sphere(oid++, 0, vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0)));
	list[i++] = new sphere(oid++, 0, vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0));
	list[i++] = new sphere(oid++, 0, vec3(-1, 0, -1), 0.5, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(oid++, 0, vec3(-1, 0, -1), -0.499, new dielectric(1.5, vec3(1, 1, 1)));
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
	image->min_z_depth = 5;
	image->max_z_depth = 25;
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
    int oid = 0;
	int n = 500;
	object **list = new object*[n + 5];
	object **templist = new object*[n + 5];

	list[i++] = new sphere(oid++, 0, vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	list[i++] = new sphere(oid++, 0, vec3(0, 1, 0), 1.0, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(oid++, 0, vec3(0, 1, 0), -0.999, new dielectric(1.5, vec3(1, 1, 1)));
	list[i++] = new sphere(oid++, 0, vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(oid++, 0, vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.5));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = drand48();
			vec3 center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					list[i++] = new sphere(oid++, 0, center, 0.2, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
				}
				else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(oid++, 0, center, 0.2,
						new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())), 0.5));
				}
				else {  // glass
					list[i++] = new sphere(oid++, 0, center, 0.2, new dielectric(1.5, vec3(1, 1, 1)));
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


void ellipse_test(object** world, object** light_list, material** matList, image_parameters* image, camera** cam) {


	//X goes right -> left
	//Y goes down -> up
	//Z goes front -> back

	image->nx = 1000;
	image->ny = 1000;
	image->ns = 100;

	image->chunk_size = 25;

	image->saveHDR = false;
	image->savePPM = false;
	image->save_name = "ellipseTest";

	//Camera
	vec3 look_from(0, 0, 1);
	vec3 look_at(0, 0, 0);
	vec3 up(0, 1, 0); // vector that is "up" for the camera
	float focal_length = (look_from - look_at).length();
	int fov = 45;
	float aperture = 0.0;
	float aspect_ratio = float(image->nx) / float(image->ny);
	*cam = new camera(look_from, look_at, up, fov, aspect_ratio, aperture, focal_length);

	image->background_colour = new vec3(0, 0, 0);

	//Iterators
	int i = 0;
	int oid = 0;
	int matNo = 0;
	object** list = new object * [50];

	//Materials
	diffuse_light* light = new diffuse_light(new vec3(10, 10, 10));
	matList[matNo++] = light;
	lambertian* red = new lambertian(vec3(0.65, 0.05, 0.05));
	matList[matNo++] = red;
	lambertian* white = new lambertian(vec3(0.73, 0.73, 0.73));
	matList[matNo++] = white;

	//Light List. Remember to remove objects from Light list, can cause issues when rendering lambertians 
	int j = 0;
	object** llist = new object * [50];
	llist[j++] = new sphere(oid++, 0, vec3(0, 0, 2), 0.5, 0);

	//Assign light list
	*light_list = new object_list(llist, j);

	//Object list
	list[i++] = new sphere(oid++, 0, vec3(0, 0, 2), 0.5, light);

	list[i++] = new ellipse(oid++, 0, vec3(0, 0, 0), vec3(0.15, 0, 0), vec3(0, -0.1, 0), red);

	list[i++] = new xy_rect(oid++, 0, -0.5, 0.5, -0.5, 0.5, -0.1, vec3(0, 0, 1), white);



	//Assign world list
	*world = new object_list(list, i);
}


#endif // !SCENES_H
