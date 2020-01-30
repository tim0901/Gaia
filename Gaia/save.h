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
#include <string>

void save(image_parameters* image) {
	
	if (image->saveHDR == false && image->savePPM == false) {
		return;
	}

	std::cout << "Saving..." << std::endl;

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
	
	
	std::cout << "All saves complete" << std::endl;
	
}

#endif // !SAVE_H
