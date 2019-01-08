#pragma once

#ifndef SAVE_H
#define SAVE_H

#include <iostream>
#include "image_parameters.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
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

		//Required for console output, breaks writing to console
		FILE *stream;

		//Sends console output to file 
		std::string ppm_name = image->save_name + ".ppm";
		freopen_s(&stream, &ppm_name[0u], "w", stdout);

		//PPM file header
		std::cout << "P3\n" << image->nx << " " << image->ny << "\n255\n";

		//Outputs colours as required by file format
		for (int j = image->ny - 1; j >= 0; j--) {
			for (int i = 0; i < image->nx; i++) {

				int ir = 255.99*image->output_array[((i + (image->nx * j)) * 4)];
				int ig = 255.99*image->output_array[((i + (image->nx * j)) * 4) + 1];
				int ib = 255.99*image->output_array[((i + (image->nx * j)) * 4) + 2];

				std::cout << ir << " " << ig << " " << ib << "\n";
			}
		}

		//Closes out.ppm file at end out output.
		fclose(stdout);

	}
	else {
		std::cout << "All saves complete" << std::endl;
	}
}

#endif // !SAVE_H
