#pragma once

#ifndef CHUNK_H
#define CHUNK_H

#include <list>
//defines a chunk as a container for the start and end x and y dimensions

class chunk {
public:

	int position[4];

	chunk() {};
	chunk(int startX, int endX, int startY, int endY) { position[0] = startX; position[1] = endX; position[2] = startY; position[3] = endY; }

	inline int startX() const { return position[0]; }
	inline int endX() const { return position[1]; }
	inline int startY() const { return position[2]; }
	inline int endY() const { return position[3]; }
};

#endif //CHUNK_H