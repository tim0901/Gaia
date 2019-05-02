#pragma once

#include "pch.h"
#include "image_parameters.h"

void initialise(image_parameters *image) {


	//We use a single sample per pixel for edge lines, so ns = 1.
	if (image->edge_line_pass == true) {

		image->ns = 1;
	}

	//Precalculate divisions for samples
	*image->sample_reciprocals_ptr = (float*)calloc(image->ns + 1, sizeof(float));
	image->sample_reciprocals[0] = 0;
	for (float x = 1; x <= image->ns; x++) {

		image->sample_reciprocals[int(x)] = 1 / x;
	}


	//Defines array for output
	*image->output_array_ptr = (float*)calloc((image->nx + 2) * (image->ny + 2) * 4, sizeof(float));


	//Splits the image into n*n squares, each to be rendered by an independent thread

	image->xChunks = ((image->nx + image->chunk_size - 1) / image->chunk_size);
	image->yChunks = ((image->ny + image->chunk_size - 1) / image->chunk_size);

	//Add chunks to render list;

	for (int i = 0; i < image->xChunks; i++) {
		for (int j = 0; j < image->yChunks; j++) {

			chunk tempChunk = chunk(image->nx - i * image->chunk_size - 1, image->nx - (i + 1)*image->chunk_size, image->ny - j * image->chunk_size - 1, image->ny - (j + 1)*image->chunk_size);

			//Deals with edges of strangely sized images
			if (tempChunk.endX() < 0) {

				tempChunk = chunk(tempChunk.startX(), 0, tempChunk.startY(), tempChunk.endY());

			}
			if (tempChunk.endY() < 0) {

				tempChunk = chunk(tempChunk.startX(), tempChunk.endX(), tempChunk.startY(), 0);

			}

			image->chunk_list.push_front(tempChunk);
		}
	}

	//Stores the number of chunks remaining to be rendered
	*image->chunks_remaining = image->xChunks * image->yChunks;

	//Initialise iterator
	image->iter = image->chunk_list.begin();
}