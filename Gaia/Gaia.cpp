#pragma once

#include "pch.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <list>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "pi.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "OpenGL.h"
#include "chunk.h"
#include "camera.h"
#include "ray.h"
#include "radiance.h"
#include "scenes.h"
#include "save.h"

//Forward declarations
void render(bool *window_open, image_parameters *image, int k, object **world, camera **cam_Ptr);

//Defines number of threads of system
unsigned const int nthreads = 8;// std::thread::hardware_concurrency();

std::string version_number = "0.1.1";

//Declares thread array
std::thread t[nthreads + 1];

std::mutex console_mutex;
std::mutex chunk_list_mutex;

int main()
{
	std::cout << "Gaia version: " << &version_number[0u] << std::endl;

	//Start timer - setup
	auto start = std::chrono::steady_clock::now();

	//Image container
	image_parameters* image = new image_parameters();

	//Container for objects to hit
	object* world;
	camera* cam;

	//Fetch scene
	cornell_box(&world, image, &cam);

	//Initialise image container
	initialise(image);

	if (image->saveHDR == false && image->savePPM == false) {
		std::cout << "No save mode selected." << std::endl;
	}

	//Seed random number generator
	srand(time(0));

	//For checking if the viewport should be open
	bool *window_open = new bool(true);
	//Initialise viewport on the last thread
	t[nthreads] = std::thread(initialise_window, image, window_open);
	*window_open = true;

	//End timer - setup
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Setup time: " << std::chrono::duration <double, std::milli>(diff).count() << "ms" << std::endl;

	//Start timer - frame
	start = std::chrono::steady_clock::now();

	//Renders out the chunks
	for (int k = 0; k < (nthreads); k++) {
		//t[k] = std::thread(render, image, k, image->chunks_remaining, window_open, &world, &light_list, &cam);
		t[k] = std::thread(render, window_open, image, k, &world, &cam);
	}

	//Joins rendering threads
	for (int k = 0; k < nthreads; ++k) {
		t[k].join();
	}

	//Saves only if the render completed
	if (*image->chunks_remaining == 0 || image->previous_samples == image->ns) {
		t[0] = std::thread(save, image);
		t[0].join();
	}

	//End timer - frame
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "Frame render time: " << std::chrono::duration <double, std::milli>(diff).count() << "ms" << std::endl;



	//Closes OpenGL render thread
	t[nthreads].join();

	//Cleanup
	terminate_window();
	delete[] world;
	delete[] image->output_array;
	delete[] image->sample_reciprocals;
	delete window_open;
}

void render(bool *window_open, image_parameters *image, int k, object **world, camera **cam_Ptr) {

	camera cam = **cam_Ptr;
	chunk to_be_rendered;
	vec4 col(0, 0, 0, 0);

	//Render edge lines only
	if (image->edge_line_pass == true) {

		//We use a single sample per pixel for edge lines
		image->ns = 1;

		//While there are chunks to be rendered
		while (*image->chunks_remaining > 0 && *window_open && image->chunk_list.end() != image->iter) {

			//Stops other threads from grabbing a chunk
			chunk_list_mutex.lock();

			//Takes chunk
			to_be_rendered = *image->iter;

			//Iterator
			std::advance(image->iter, 1);

			//Unlock mutex
			chunk_list_mutex.unlock();

			//For each pixel
			for (int j = int(to_be_rendered.startY()); j >= int(to_be_rendered.endY()); j--) {
				for (int i = int(to_be_rendered.endX()); i <= int(to_be_rendered.startX()); i++) {
					col = vec4(0, 0, 0, 0);

					//Send central sample ray

					//Shoot ray through center of pixel;
					float u = float(i + 0.5) / float(image->nx);
					float v = float(j + 0.5) / float(image->ny);
					ray r = cam.get_ray(u, v);

					//Information from sample ray required for later calculations - container is reused. 
					hit_record rec;
					rec = edge_cast(image, r, *world, 0);
					int sample_object_id = rec.object_id;
					int sample_primitive_id = rec.primitive_id;
					//std::cout << sample_object_id << std::endl;
					float sample_t = rec.t;

					//N - the total number of rays sent including the sample ray.
					float total_rays = (2 * image->edge_line_quality_n + 1)*(2 * image->edge_line_quality_n + 1);

					//m is the number of rays that have struck an object different to the sample ray. 
					int m = 0;

					//d is the number of stencil rays with a significant length difference from the sample ray
					int d = 0;

					//Measure of how strong the edge is
					float edge_metric = 0;

					//Vector containing length of rays cast to find self occluding silhouettes
					std::vector<float> ray_t;

					//Sends rays for ray stencil

					//N rings
					for (int k = 1; k <= image->edge_line_quality_n; k++) {
						//8N Rays per ring
						for (int l = 0; l < 8 * k; l++) {
							//radius = h therefore length of vector = h/N.
							float radius = image->edge_line_thickness_h / float(image->edge_line_quality_n);

							//Angle between each ray = 2Pi/8N radians
							float theta = 2 * M_PI / float(8 * image->edge_line_quality_n);

							//Ray position
							float rx = radius * sin(l * theta);
							float ry = radius * cos(l * theta);

							//std::cout << "l: " << l << " " << "l*theta: " << l*theta << " " << "rx: " << rx << " " << "ry: " << ry << std::endl;

							u = float(i + 0.5 + rx) / float(image->nx);
							v = float(j + 0.5 + ry) / float(image->ny);
							r = cam.get_ray(u, v);

							//Casts ray, recieves hit record
							rec = edge_cast(image, r, *world, 0);

							ray_t.push_back(rec.t);

							if (rec.object_id != sample_object_id) {
								m++;
							}
							else if (rec.primitive_id != sample_primitive_id) {
								m++;
							}

							if (*window_open == false) {
								break;
							}
						}
						if (*window_open == false) {
							break;
						}
					}

					//Detect edges
					if (m == 0) {
						//All rays hit the same object - only possible lines are crease edges or self-occluding silhouettes

						//TODO: 2D Finite difference stencil to search for crease edges

						//Find self-occluding silhouettes
						for (int k = 0; k < total_rays - 1; k++) {
							//The void cannot have a self-occluding silhouette.
							if (sample_object_id == -1) {
								break;
							}

							d = 0;
							float T = sample_t / 100;//Arbitrarily chosen

							if (abs(ray_t[k] - sample_t) >= T) {
								d++;
							}
						}

						//Calculate the edge metric
						edge_metric = 1 - ((abs(d - 0.5*total_rays)) / (0.5*total_rays));

					}
					else {
						//Some of the rays hit a different object - we have a sillhouette edge or an intersection line!
						//Calculate the edge metric
						edge_metric = 1 - ((abs(m - 0.5*total_rays)) / (0.5*total_rays));

					}

					//Output final pixel colour
					col[0] = col[1] = col[2] = 1 - edge_metric;

					col[3] = 1; //Setting alpha to 1 for all pixels in edge line renders.

					//Colour correction
					col = vec4(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()), col.a());

					//Save pixel colour to array
					image->output_array[(i + (image->nx * j)) * 4] = col[0];
					image->output_array[((i + (image->nx * j)) * 4) + 1] = col[1];
					image->output_array[((i + (image->nx * j)) * 4) + 2] = col[2];

					//Alpha channel currently unused - sets as 1 for all pixels
					image->output_array[((i + (image->nx * j)) * 4) + 3] = col[3];

					if (*window_open == false) {
						break;
					}
				}
				if (*window_open == false) {
					break;
				}
			}

			//Updates number of remaining chunks
			*image->chunks_remaining = *image->chunks_remaining - 1;

			if (*window_open == false) {
				break;
			}
		}
	}
	if (image->z_depth_pass == true) {

		//While there are chunks to be rendered
		while (*image->chunks_remaining > 0 && *window_open && image->chunk_list.end() != image->iter) {

			//Stops other threads from grabbing a chunk
			chunk_list_mutex.lock();

			//Takes chunk
			to_be_rendered = *image->iter;

			//Iterator
			std::advance(image->iter, 1);

			//Unlock mutex
			chunk_list_mutex.unlock();

			//For each pixel
			for (int j = int(to_be_rendered.startY()); j >= int(to_be_rendered.endY()); j--) {
				for (int i = int(to_be_rendered.endX()); i <= int(to_be_rendered.startX()); i++) {

					col = vec4(0, 0, 0, 0);

					for (int k = 0; k < image->ns; k++) {

						//Randomly generate rays in pixel
						float u = float(i + drand48()) / float(image->nx);
						float v = float(j + drand48()) / float(image->ny);
						ray r = cam.get_ray(u, v);

						//Information from ray required for later calculations - container is reused. 
						hit_record rec;
						rec = edge_cast(image, r, *world, 0);

						float distance = (rec.p - cam.origin).length();

						//Check for invert z-depth colours flag
						if (image->invert_z_colours == true) {
							//Rays going to infinity
							if (rec.object_id == -1) {
								col += vec4(0, 0, 0, 0);
							}
							//Objects too close to camera
							else if (distance <= image->min_z_depth) {
								col += vec4(1, 1, 1, 1);
							}
							//Objects too far from camera
							else if (distance >= image->max_z_depth) {
								col += vec4(0, 0, 0, 1);
							}
							else {
								float colour = 1 - ((distance - image->min_z_depth) / (image->max_z_depth - image->min_z_depth));
								col += vec4(colour, colour, colour, 1);
							}
						}
						else {
							//Rays going to infinity
							if (rec.object_id == -1) {
								col += vec4(1, 1, 1, 0);
							}
							//Objects too close to camera
							else if (distance <= image->min_z_depth) {
								col += vec4(0, 0, 0, 1);
							}
							//Objects too far from camera
							else if (distance >= image->max_z_depth) {
								col += vec4(1, 1, 1, 1);
							}
							else {
								float colour = (distance - image->min_z_depth) / (image->max_z_depth - image->min_z_depth);
								col += vec4(colour, colour, colour, 1);
							}
						}
						
					}
					
					col[0] = col[0] / image->ns;
					col[1] = col[1] / image->ns;
					col[2] = col[2] / image->ns;

					if (col.a() > 0) {
						col[3] = 1;
					}
					else {
						col[3] = 0;
					}

					//Colour correction
					col = vec4(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()), col.a());

					//Save pixel colour to array
					image->output_array[(i + (image->nx * j)) * 4] = col[0];
					image->output_array[((i + (image->nx * j)) * 4) + 1] = col[1];
					image->output_array[((i + (image->nx * j)) * 4) + 2] = col[2];

					//Alpha channel currently unused
					image->output_array[((i + (image->nx * j)) * 4) + 3] = col[3];

					if (*window_open == false) {
						break;
					}
				}
				if (*window_open == false) {
					break;
				}
			}

			//Updates number of remaining chunks
			*image->chunks_remaining = *image->chunks_remaining - 1;

			if (*window_open == false) {
				break;
			}
		}
	}
	//Beauty Render
	else {

		//Non-iterative rendering
		if (image->iterative_mode == false) {

			//While there are chunks to be rendered
			while (*image->chunks_remaining > 0 && *window_open && image->chunk_list.end() != image->iter) {

				//Stops other threads from grabbing a chunk
				chunk_list_mutex.lock();

				//Takes chunk
				to_be_rendered = *image->iter;

				//Iterator
				std::advance(image->iter, 1);

				//Unlock mutex
				chunk_list_mutex.unlock();

				//For each pixel
				for (int j = int(to_be_rendered.startY()); j >= int(to_be_rendered.endY()); j--) {
					for (int i = int(to_be_rendered.endX()); i <= int(to_be_rendered.startX()); i++) {
						col = vec4(0, 0, 0, 0);

						//For each sample
						for (int s = 0; s < image->ns; s++) {

							//Generates random ray within pixel
							float u = float(i + drand48()) / float(image->nx);
							float v = float(j + drand48()) / float(image->ny);
							ray r = cam.get_ray(u, v);

							//Cast ray, gets colour
							col += de_nan(cast(image, r, *world, 0));

							if (*window_open == false) {
								break;
							}
						}

						//Divide col by number of samples the pixel has had
						col[0] *= image->sample_reciprocals[image->ns];
						col[1] *= image->sample_reciprocals[image->ns];
						col[2] *= image->sample_reciprocals[image->ns];

						//Colour correction
						col = vec4(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()), col.a());

						//Save pixel colour to array
						if(image->red_channel == true)
							image->output_array[(i + (image->nx * j)) * 4] = col[0];
						else
							image->output_array[(i + (image->nx * j)) * 4] = 0.0;

						if (image->green_channel == true)
							image->output_array[((i + (image->nx * j)) * 4) + 1] = col[1];
						else
							image->output_array[((i + (image->nx * j)) * 4) + 1] = 0.0;


						if (image->blue_channel == true)
							image->output_array[((i + (image->nx * j)) * 4) + 2] = col[2];
						else
							image->output_array[((i + (image->nx * j)) * 4) + 2] = 0.0;

						//Alpha channel currently unused - sets as 1 for all pixels if set to false
						if (image->alpha_channel == true)
							image->output_array[((i + (image->nx * j)) * 4) + 3] = col[3];
						else 
							image->output_array[((i + (image->nx * j)) * 4) + 3] = 1;

						if (*window_open == false) {
							break;
						}
					}
					if (*window_open == false) {
						break;
					}
				}

				//Updates number of remaining chunks
				*image->chunks_remaining = *image->chunks_remaining - 1;

				if (*window_open == false) {
					break;
				}
			}
		}
		//Iterative rendering
		else {
			int chunk_previous_samples = 0;
			int chunk_sample_number = 1;

			while (image->previous_samples < image->ns && *window_open) {

				//Stops other threads from grabbing a chunk
				chunk_list_mutex.lock();

				//Resets iterator if it reaches the end
				if (image->chunk_list.end() == image->iter)
				{
					image->iter = image->chunk_list.begin();

					//Increments the "sample number" stored by image_parameters
					image->sample_number++;
					image->previous_samples++;
				}
				//Checks for oversampling
				if (image->previous_samples == image->ns) {
					chunk_list_mutex.unlock();
					break;
				}

				//Stores locally the sample number. Used for chunks at the end of the list,which may finish after image_parameters->samplenumber has been iterated.
				chunk_previous_samples = image->previous_samples;
				chunk_sample_number = image->sample_number;

				//Takes chunk
				to_be_rendered = *image->iter;

				//Advance iterator
				std::advance(image->iter, 1);

				//Unlock mutex
				chunk_list_mutex.unlock();
				//For each pixel
				for (int j = int(to_be_rendered.startY()); j >= int(to_be_rendered.endY()); j--) {
					for (int i = int(to_be_rendered.endX()); i <= int(to_be_rendered.startX()); i++) {
						//std::cout << i << " " << j << std::endl;
						//Load existing pixel colour
						col[0] = image->output_array[(i + (image->nx * j)) * 4];
						col[1] = image->output_array[((i + (image->nx * j)) * 4) + 1];
						col[2] = image->output_array[((i + (image->nx * j)) * 4) + 2];
						col[3] = image->output_array[((i + (image->nx * j)) * 4) + 3];

						//Undo colour correction, multiplies by number of previous samples
						col = vec4((col.r() * col.r())* chunk_previous_samples, (col.g() * col.g())*chunk_previous_samples, (col.b() * col.b())*chunk_previous_samples, col.a());

						//Send ray
						float u = float(i + drand48()) / float(image->nx);
						float v = float(j + drand48()) / float(image->ny);
						ray r = cam.get_ray(u, v);
						col += de_nan(cast(image, r, *world, 0));

						//Divide col by number of samples the pixel has had
						col[0] *= image->sample_reciprocals[chunk_sample_number];
						col[1] *= image->sample_reciprocals[chunk_sample_number];
						col[2] *= image->sample_reciprocals[chunk_sample_number];

						//Colour correction
						col = vec4(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()), col.a());

						//Save pixel colour to array
						if (image->red_channel == true)
							image->output_array[(i + (image->nx * j)) * 4] = col[0];
						else
							image->output_array[(i + (image->nx * j)) * 4] = 0.0;

						if (image->green_channel == true)
							image->output_array[((i + (image->nx * j)) * 4) + 1] = col[1];
						else
							image->output_array[((i + (image->nx * j)) * 4) + 1] = 0.0;

						if (image->blue_channel == true)
							image->output_array[((i + (image->nx * j)) * 4) + 2] = col[2];
						else
							image->output_array[((i + (image->nx * j)) * 4) + 2] = 0.0;

						//Alpha channel currently unused - sets as 1 for all pixels if set to false
						if (image->alpha_channel == true)
							image->output_array[((i + (image->nx * j)) * 4) + 3] = col[3];
						else
							image->output_array[((i + (image->nx * j)) * 4) + 3] = 1;

						//std::cout << i << " " << j << std::endl;
						if (*window_open == false) {
							break;
						}
					}
					if (*window_open == false) {
						break;
					}
				}

				if (*window_open == false) {
					break;
				}
			}
		}
	}

	//Close thread
	console_mutex.lock();
	std::cout << "Thread: " << k << " finished." << std::endl;
	console_mutex.unlock();
}