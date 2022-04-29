#include "Tiler.h"

// Sort the generated tiles into a spiral
void Tiler::Spiral(std::vector<Tile>& tiles, size_t xDim, size_t yDim, size_t tileDimension) {

	// Check that a tile at the current coordinates would exist within the image
	auto IsTileValid = [tileDimension, xDim, yDim](int x, int y) {

		int xStart = x * tileDimension;
		int yStart = y * tileDimension;

		if (xStart < 0 || yStart < 0)
			return false;
		if (xStart >= xDim || yStart >= yDim)
			return false;

		return true;
	};

	// How many tiles exist in each dimension
	size_t xTiles = (xDim + tileDimension - 1) / tileDimension;
	size_t yTiles = (yDim + tileDimension - 1) / tileDimension;

	std::vector<Tile> sortedTiles;
	int xLength = 1;
	int yLength = 1;

	// Start in the middle
	int x = xTiles / 2;
	int y = yTiles / 2;

	// Insert first tile 
	sortedTiles.push_back(tiles.at(x + y * xTiles));

	while (sortedTiles.size() < tiles.size()) {

		// Up
		for (int i = y + 1; i <= y + yLength; i++) {

			// Insert tile if valid
			if (IsTileValid(x, i))
				sortedTiles.push_back(tiles.at(x + i * xTiles));
		}
		y = y + yLength;
		yLength++;

		// Right
		for (int j = x + 1; j <= x + xLength; j++) {
			if (IsTileValid(j, y))
				sortedTiles.push_back(tiles.at(j + y * xTiles));
		}
		x = x + xLength;
		xLength++;

		// Down
		for (int i = y - 1; i >= y - yLength; i--) {
			if (IsTileValid(x, i))
				sortedTiles.push_back(tiles.at(x + i * xTiles));
		}
		y = y - yLength;
		yLength++;

		// Left
		for (int j = x - 1; j >= x - xLength; j--) {
			if (IsTileValid(j, y))
				sortedTiles.push_back(tiles.at(j + y * xTiles));
		}
		x = x - xLength;
		xLength++;

	}

	tiles.clear();
	tiles = sortedTiles;
	sortedTiles.clear();
}