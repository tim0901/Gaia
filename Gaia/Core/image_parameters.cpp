#include "image_parameters.h"

void spiral(image_parameters* image);
void topright(image_parameters *image, std::vector<chunk> *temp_chunk_vector, int x1, int x2, int y1, int y2);
void bottomleft(image_parameters *image, std::vector<chunk> *temp_chunk_vector, int x1, int x2, int y1, int y2);


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

	if (image->generateHeatMap) {
		*image->heatMapArrayPtr = (float*)calloc((image->nx + 2) * (image->ny + 2), sizeof(float));
	}



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

			image->chunk_vector.push_back(tempChunk);
		}
	}

	//Stores the number of chunks remaining to be rendered - also used in spiral sorting algorithm
	*image->chunks_remaining = image->xChunks * image->yChunks;

	//Sets normal render order
	std::reverse(image->chunk_vector.begin(), image->chunk_vector.end());
	
	if (image->spiral == true | image->spiral_in == true) {
		spiral(image);
	}

	//Initialise iterator
	image->vec_iter = image->chunk_vector.begin();

	std::cout << image->nx << "x" << image->ny << " at " << image->ns << " samples per pixel" << std::endl;

	if (image->saveHDR == false && image->savePPM == false) {
		std::cout << "No save mode selected" << std::endl;
	}
	else {
		std::cout << "File name: " << image->save_name << std::endl;
		std::cout << "Format(s): ";
		if(image->saveHDR) {
			std::cout << ".hdr ";
		}
		if (image->savePPM) {
			std::cout << ".ppm ";
		}
		std::cout << " " << std::endl;
	}
    
    //Set image as initialized
    image->isInitialized = true;
}


void spiral(image_parameters *image){
	//Converts default chunk ordering into a clockwise spiral from the centre

	//Temporary vector for sorting
	std::vector<chunk> *temp_chunk_vector = new std::vector<chunk>;

	//Spiral algorithm
	topright(image, temp_chunk_vector, 0, image->xChunks-1, 0, image->yChunks-1);

	//Clear container
	image->chunk_vector.clear();

	//Reverse spiral if wanting in->out
	if (image->spiral_in == false) {
		
		for (int i = 0; i < *image->chunks_remaining; i++) {

			image->chunk_vector.push_back(temp_chunk_vector->at(*image->chunks_remaining - 1 -i));
		}
	}
	else {
		image->chunk_vector = *temp_chunk_vector;
	}

	//Clear temp vector
	temp_chunk_vector->clear();
	delete temp_chunk_vector;
}

void topright(image_parameters *image,std::vector<chunk> *temp_chunk_vector, int x1, int x2, int y1, int y2) {
	int i = 0;
	int j = 0;
	//Push top
	for (i = x1; i <= x2; i++) {
		//std::cout << "T: " << i << " " << j << std::endl;
		temp_chunk_vector->push_back(image->chunk_vector.at(y1 + i * image->yChunks));
	}
	//push right
	for (j = y1 + 1; j <= y2; j++) {
		//std::cout << "R: " << i << " " << j << std::endl;
		temp_chunk_vector->push_back(image->chunk_vector.at(j + x2 * image->yChunks));
	}

	if (x2 - x1 > 0) {
		bottomleft(image, temp_chunk_vector, x1, x2 - 1, y1 + 1, y2);
	}
}

void bottomleft(image_parameters *image, std::vector<chunk> *temp_chunk_vector, int x1, int x2, int y1, int y2) {
	int i = 0;
	int j = 0;
	//Push bottom
	for (i = x2; i >= x1; i--) {
		//std::cout << "B: " << i << " " << j << std::endl;
		temp_chunk_vector->push_back(image->chunk_vector.at(y2 + i * image->yChunks));
	}
	//push left
	for (j = y2-1; j >= y1; j--) {
		//std::cout << "L: " << i << " " << j << std::endl;
		temp_chunk_vector->push_back(image->chunk_vector.at(j + x1* image->yChunks));
	}

	if ((x2 - x1) > 0) {
		topright(image, temp_chunk_vector, x1+1 , x2, y1, y2-1);
	}
}


