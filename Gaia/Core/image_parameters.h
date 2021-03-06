#pragma once

#ifndef IMAGE_PARAMETERS_H
#define IMAGE_PARAMETERS_H

#include "vec3.h"
#include"chunk.h"
#include <vector>
#include <chrono>

struct image_parameters {

    //Render timer
	std::chrono::time_point<std::chrono::steady_clock> timerStartPoint;
	std::chrono::time_point<std::chrono::steady_clock> timerEndPoint;
	std::chrono::duration<double> duration;

    //Is this struct initialized?
    bool isInitialized = false;
    
	//Viewport
	bool show_viewport = true;
	bool progress_monitoring = false; // Prints chunks remaining in non-iterative mode

    
    //Thread iterator
    int currentActiveThreads = 0;

	//Render Options

	///Image dimensions
	int nx = 1000;
	int ny = 500;
    
    int maxDepth = 50; // maximum number of bounces in the scene.

	///Samples per pixel
	int ns = 100;
	float *sample_reciprocals = { 0 };
	float **sample_reciprocals_ptr = &sample_reciprocals;
	int stratify_divisions = 1;
	int strat_sample_iter_u = 0;
	int strat_sample_iter_v = 0;

	///Ray bounces
	int bounces = 10;

	///Chunk order
	bool spiral = true;
	bool spiral_in = false;

	///Chunk storage vector
	std::vector<chunk> chunk_vector;
	std::vector<chunk>::iterator vec_iter;

	///Chunk size
	int chunk_size = 50;
	int xChunks;
	int yChunks;

	///Pass Options
	bool edge_line_pass = false;
	bool z_depth_pass = false;
	bool generateHeatMap = false;

	///Edge Line Pass Options
	int edge_line_quality_n = 3; //+ve integer, min N=1
	float edge_line_thickness_h = 3;
	bool primitive_edge_lines = true;
	vec3* background_colour = new vec3(0, 0, 0);

	///Z_depth Pass Options
	float min_z_depth = 5;
	float max_z_depth = 25;
	bool invert_z_colours = false;
	
	///Iterative mode settings
	bool iterative_mode = false;
	int sample_number = 1;
	int previous_samples = 0;
	int* chunks_remaining = new int(0);

	///OpenSubDiv settings
	int maxlevel = 0; // 0 = no subdivision



	//Output Options

	///Array of rendered pixels
	float *output_array = { 0 };
	float **output_array_ptr = &output_array;

	float* heatMapArray = { 0 };
	float** heatMapArrayPtr = &heatMapArray;

	///Isolate output channels - only for beauty renders. 
	bool red_channel = true;
	bool green_channel = true;
	bool blue_channel = true;
	bool alpha_channel = true; // None of the output file formats support alpha channel data, so this option is currently useless.

	///File Options
	float gamma = 2.2;
	std::string save_name = "random edge";
	bool savePPM = false;
	bool saveHDR = false;
    
    //Destructor
    ~image_parameters(){
        std::cout << "Delete image arrays" << std::endl;
        delete background_colour;
        delete chunks_remaining;
        if(isInitialized == true){
            free(sample_reciprocals);
            free(output_array);
			if (generateHeatMap) {
				free(heatMapArray);
			}
        }
    }
    
};

void initialise(image_parameters *image);

#endif //IMAGE_PARAMETERS_H
