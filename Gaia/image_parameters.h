#pragma once

#ifndef IMAGE_PARAMETERS_H
#define IMAGE_PARAMETERS_H

#include "vec3.h"
#include"chunk.h"
#include <vector>

struct image_parameters {

	//Image dimensions
	int nx = 1000;
	int ny = 500;

	//Number of Samples
	int ns = 100;
	float *sample_reciprocals = { 0 };
	float **sample_reciprocals_ptr = &sample_reciprocals;

	//Chunk order
	bool spiral = true;
	bool spiral_in = false;

	//Sets size of chunk
	int chunk_size = 50;
	int xChunks;
	int yChunks;

	//Output Storage
	float *output_array = { 0 };
	float **output_array_ptr = &output_array;

	std::vector<chunk> chunk_vector;
	std::vector<chunk>::iterator vec_iter;

	//Rendering Options
	bool montecarlo = false;
	bool edge_line_pass = false;
	bool z_depth_pass = false;

	//Isolate output channels - only for beauty renders. 
	bool red_channel = true;
	bool green_channel = true;
	bool blue_channel = true;
	bool alpha_channel = true; // None of the output file formats support alpha channel data, so this option is currently useless.

	//Z_depth Pass Options
	float min_z_depth = 5;
	float max_z_depth = 25;
	bool invert_z_colours = false;

	//Edge Line Pass Options
	int edge_line_quality_n = 3; //+ve integer, min N=1
	float edge_line_thickness_h = 3;
	vec3 *background_colour = new vec3(0, 0, 0);
	
	//Iterative mode settings
	bool iterative_mode = true;
	int sample_number = 1;
	int previous_samples = 0;
	int* chunks_remaining = new int(0);

	//Save Options
	std::string save_name = "random edge";
	bool savePPM = false;
	bool saveHDR = false;
};

void initialise(image_parameters *image);

#endif //IMAGE_PARAMETERS_H