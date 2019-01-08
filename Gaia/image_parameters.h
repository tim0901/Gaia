#pragma once

#ifndef IMAGE_PARAMETERS_H
#define IMAGE_PARAMETERS_H

#include "vec3.h"
#include"chunk.h"

struct image_parameters {

	//Image dimensions
	int nx = 1000;
	int ny = 500;

	//Number of Samples
	int ns = 1000;
	float *sample_reciprocals = { 0 };
	float **sample_reciprocals_ptr = &sample_reciprocals;

	//Sets size of chunk
	int chunk_size = 50;
	int xChunks;
	int yChunks;

	//Output Storage
	float *output_array = { 0 };
	float **output_array_ptr = &output_array;
	std::list<chunk> chunk_list;
	std::list<chunk>::iterator iter;

	//Rendering Options
	bool montecarlo = false;
	bool constant_luminance = false;
	bool edge_line_pass = false;

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
	std::string save_name = "v003 gooch render";
	bool savePPM = false;
	bool saveHDR = true;
};

void initialise(image_parameters *image);

#endif //IMAGE_PARAMETERS_H