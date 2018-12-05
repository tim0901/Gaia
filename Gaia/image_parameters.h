#pragma once

#ifndef IMAGE_PARAMETERS_H
#define IMAGE_PARAMETERS_H

#include"chunk.h"
#include <list>

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

	//Iterative mode settings
	bool iterative_mode = true;
	int sample_number = 1;
	int previous_samples = 0;
	int* chunks_remaining = new int(0);

	//Save Options
	std::string save_name = "v_002_cornell_box";
	bool savePPM = false;
	bool saveHDR = true;
};

void initialise(image_parameters *image);

#endif //IMAGE_PARAMETERS_H