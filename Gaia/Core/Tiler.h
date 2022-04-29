#ifndef TILER_H
#define TILER_H

#include <memory>
#include <mutex>
#include <vector>
#include "../Memory/CircularBuffer.h"

class Tile {
public:
	Tile() {}
	Tile(int xS, int xE, int  yS, int  yE, int s) : xStart(xS), xEnd(xE), yStart(yS), yEnd(yE), samples(s) {}
	int xStart = 0;
	int xEnd = 0;
	int yStart = 0;
	int yEnd = 0;
	int samples = 0;
};

inline std::ostream& operator << (std::ostream& os, const Tile& t) {
	os << "xStart: " << t.xStart << " xEnd: " << t.xEnd << " yStart: " << t.yStart << " yEnd: " << t.yEnd << " Samples: " << t.samples;
	return os;
}

// Creates a list of tiles to be rendered 
class Tiler
{
public:
	Tiler() {}

	// Fetch the next tile from the circular buffer
	bool FetchTile(Tile *tile) {
		// Lock mutex
		tilerMutex.lock();
		if (tileListIterator < numberOfTiles * rendersPerTile) {
			// Still rendering to be done
			*tile = buffer.ReturnNextElement();
			tileListIterator++;
			tilerMutex.unlock();

			return true;
		}
		else {
			// We're done here
			tilerMutex.unlock();
			return false;
		}
	}


protected:
	void Spiral(std::vector<Tile>& tiles, size_t xDim, size_t yDim, size_t tileDimension);

	Vector<size_t, 2> imageDimensions;

	CircularBuffer<Tile> buffer;
	std::mutex tilerMutex;

	size_t tileListIterator = 0;
	size_t numberOfTiles = 0;
	size_t rendersPerTile = 0;
};

// Render tiles one sample at a time
class IterativeTiler : public Tiler {
public:
	IterativeTiler(size_t xDim, size_t yDim, size_t tileDimension, size_t totalSamples) {
		
		// Set size of image.
		this->imageDimensions.x() = xDim;
		this->imageDimensions.y() = yDim;

		std::vector<Tile> tiles;
		size_t xEnd = tileDimension, yEnd = tileDimension;
		for (size_t y = 0; y < yDim; y += tileDimension) {
			for (size_t x = 0; x < xDim; x += tileDimension) {

				// Shrink tiles on the edge to ensure they are within bounds.
				if (xEnd >= xDim) {
					xEnd = xDim;
				}
				if (yEnd >= yDim) {
					yEnd = yDim;
				}
				tiles.push_back(Tile(static_cast<int>(x), static_cast<int>(xEnd), static_cast<int>(y), static_cast<int>(yEnd), 1));

				xEnd += tileDimension;
			}
			xEnd = tileDimension;
			yEnd += tileDimension;
		}

		Spiral(tiles, xDim, yDim, tileDimension);

		// Insert tile list into the circular buffer
		this->buffer.OverrideData(tiles);
		this->numberOfTiles = buffer.size();
		this->rendersPerTile = totalSamples;
	}
};

// Render each tile in its entirety
class BatchTiler : public Tiler {
public:
	BatchTiler(size_t xDim, size_t yDim, size_t tileDimension, size_t totalSamples) {

		// Set size of image.
		this->imageDimensions.x() = xDim;
		this->imageDimensions.y() = yDim;

		std::vector<Tile> tiles;
		size_t xEnd = tileDimension, yEnd = tileDimension;
		for (size_t y = 0; y < yDim; y += tileDimension) {
			for (size_t x = 0; x < xDim; x += tileDimension) {

				// Shrink tiles on the edge to ensure they are within bounds.
				if (xEnd >= xDim) {
					xEnd = xDim;
				}
				if (yEnd >= yDim) {
					yEnd = yDim;
				}
				tiles.push_back(Tile(static_cast<int>(x), static_cast<int>(xEnd), static_cast<int>(y), static_cast<int>(yEnd), static_cast<int>(totalSamples)));

				xEnd += tileDimension;
			}
			xEnd = tileDimension;
			yEnd += tileDimension;
		}

		Spiral(tiles, xDim, yDim, tileDimension);

		// Insert tile list into the circular buffer
		this->buffer.OverrideData(tiles);
		this->numberOfTiles = buffer.size();
		this->rendersPerTile = 1;
	}
};

#endif // !TILER_H

