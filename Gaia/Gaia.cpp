#pragma once

#include "pch.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include "pi.h"
#include "vec3.h"
#include "vec2.h"
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

std::string version_number = "0.0.3";

//Declares thread array
std::thread t[nthreads + 1];

std::mutex console_mutex;
std::mutex chunk_list_mutex;

int main()
{
	std::cout << "Gaia version: " << &version_number[0u] << std::endl;

	//Start timer - setup
	auto start = std::chrono::steady_clock::now();

	//Initialise image
	image_parameters* image = new image_parameters();
	initialise(image);

	//Seed random number generator
	srand(time(0));

	//For checking if the viewport should be open
	bool *window_open = new bool(true);
	//Initialise viewport on the last thread
	t[nthreads] = std::thread(initialise_window, image, window_open);
	*window_open = true;

	//Container for objects to hit
	object* world;
	camera* cam;

	//Fetch scene
	goochtest(&world, image, &cam);

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
	vec3 col(0, 0, 0);

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
					col = vec3(0, 0, 0);

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
					col *= image->sample_reciprocals[image->ns];

					//Colour correction
					col = vec3(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()));

					//Save pixel colour to array
					image->output_array[(i + (image->nx * j)) * 4] = col[0];
					image->output_array[((i + (image->nx * j)) * 4) + 1] = col[1];
					image->output_array[((i + (image->nx * j)) * 4) + 2] = col[2];

					//Alpha channel currently unused - sets as 1 for all pixels
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

					//Load existing pixel colour
					col[0] = image->output_array[(i + (image->nx * j)) * 4];
					col[1] = image->output_array[((i + (image->nx * j)) * 4) + 1];
					col[2] = image->output_array[((i + (image->nx * j)) * 4) + 2];

					//Undo colour correction, multiplies by number of previous samples
					col = vec3((col.r() * col.r())* chunk_previous_samples, (col.g() * col.g())*chunk_previous_samples, (col.b() * col.b())*chunk_previous_samples);

					//TODO:COLOUR CODE
					//Send ray
					float u = float(i + drand48()) / float(image->nx);
					float v = float(j + drand48()) / float(image->ny);
					ray r = cam.get_ray(u, v);
					col += de_nan(cast(image, r, *world, 0));

					//Divide col by number of samples the pixel has had
					col *= image->sample_reciprocals[chunk_sample_number];

					//Colour correction
					col = vec3(sqrt(col.r()), sqrt(col.g()), sqrt(col.b()));

					//Save pixel colour to array
					image->output_array[(i + (image->nx * j)) * 4] = col[0];
					image->output_array[((i + (image->nx * j)) * 4) + 1] = col[1];
					image->output_array[((i + (image->nx * j)) * 4) + 2] = col[2];

					//Alpha channel currently unused - sets as 1 for all pixels
					image->output_array[((i + (image->nx * j)) * 4) + 3] = 1;

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
	//Close thread
	console_mutex.lock();
	std::cout << "Thread: " << k << " finished." << std::endl;
	console_mutex.unlock();
}