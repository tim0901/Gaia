#pragma once

#ifndef SAVE_H
#define SAVE_H

#include <iostream>
#include <algorithm>
#include "image_parameters.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION

//Additional argument needed for Windows
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_WIN64)
    #define STBI_MSC_SECURE_CRT
#endif


#include "stb_image_write.h"
#include "colourGradient.h"
#include <string>

void save(image_parameters* image) {
	
	if (image->saveHDR == false && image->savePPM == false && image->generateHeatMap == false) {
		return;
	}

	std::cout << " Saving...";

	if (image->saveHDR == true) {

		//Exports as .hdr - loses alpha channel data 
		stbi_flip_vertically_on_write(1);
		std::string hdr_name = image->save_name + ".hdr";
		stbi_write_hdr(&hdr_name[0u], image->nx, image->ny, 4, image->output_array);

		std::cout << ".HDR save complete" << std::endl;
	}
	if (image->savePPM == true) {

		//Exports as .PPM - loses alpha channel data

		//Sends console output to file 
		std::string ppm_name = image->save_name + ".ppm";
		

		//Stream for writing to file
		std::ofstream ofs;

		//Open file
		ofs.open(ppm_name);

		//PPM file header
		ofs << "P3\n" << image->nx << " " << image->ny << "\n255\n";
		float gamma = 2.2;
		//Outputs colours as required by file format
		for (int j = image->ny - 1; j >= 0; j--) {
			for (int i = 0; i < image->nx; i++) {
				
				int ir = 255.99 * sqrt(powf(image->output_array[((i + (image->nx * j)) * 4)],1/gamma));
				int ig = 255.99 * sqrt(powf(image->output_array[((i + (image->nx * j)) * 4) + 1],1/gamma));
				int ib = 255.99 * sqrt(powf(image->output_array[((i + (image->nx * j)) * 4) + 2],1/gamma));

				ofs << ir << " " << ig << " " << ib << "\n";
			}
		}

		//Close file
		ofs.close();
		std::cout << ".PPM save complete" << std::endl;
	}
	if (image->generateHeatMap) {

		//Generate a heat map
		//Uses .ppm as it's easiest to set up.

		//First we need to normalize the range of values. This is why we use floats rather than ints for the array. 

		float smallest = image->heatMapArray[0];
		float largest = image->heatMapArray[0];

		//Find smallest and largest values in the array
		for (int j = image->ny - 1; j >= 0; j--) {
			for (int i = 0; i < image->nx; i++) {
				if (image->heatMapArray[(i + (image->nx * j))] < smallest) {
					smallest = image->heatMapArray[(i + (image->nx * j))];
				}
				else if (image->heatMapArray[(i + (image->nx * j))] > largest) {
					largest = image->heatMapArray[(i + (image->nx * j))];
				}
			}
		}

		//Save reciprocal for efficiency
		float inverse = 1 / (largest - smallest);

		//Set up the colour gradient
		colourGradient heatMap;
		heatMap.createDefaultHeatMapGradient();
		vec3 rgb;

		//Send console output to file 
		std::string ppm_name = image->save_name + "_heat_map.ppm";

		//Stream for writing to file
		std::ofstream ofs;

		//Open file
		ofs.open(ppm_name);

		//PPM file header
		ofs << "P3\n" << image->nx << " " << image->ny << "\n255\n";
		float gamma = 2.2;

		//Outputs colours as required by file format
		for (int j = image->ny - 1; j >= 0; j--) {
			for (int i = 0; i < image->nx; i++) {
				
				//Normalize the array values
				image->heatMapArray[(i + (image->nx * j))] = (image->heatMapArray[(i + (image->nx * j))] - smallest) * inverse; // newValue = (oldValue - smallest)/(largest-smallest)

				//Calculate the colour for them
				heatMap.returnColourForValue(image->heatMapArray[(i + (image->nx * j))], rgb);

				int ir = 255.99 * sqrt(powf(rgb.r(), 1 / gamma));
				int ig = 255.99 * sqrt(powf(rgb.g(), 1 / gamma));
				int ib = 255.99 * sqrt(powf(rgb.b(), 1 / gamma));

				rgb = vec3(0, 0, 0); // Reset 

				ofs << ir << " " << ig << " " << ib << "\n";
			}
		}

		heatMap.clearGradient();

		//Close file
		ofs.close();
		std::cout << "Heat map generated" << std::endl;

	}
	
	std::cout << "All saves complete" << std::endl;
	
}

#endif // !SAVE_H
