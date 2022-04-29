#ifndef IMAGE_H
#define IMAGE_H

#include <malloc.h>
#include <vector>
#include <string>

class Image
{
public:
	Image() {}
	Image(int nX, int nY, size_t nS, std::string name, bool iterativeSampling = false) : xDim(nX), yDim(nY), nSamples(nS), fileName(name) {
		data.resize(nX * nY * 3);
		if (iterativeSampling) {
			samplesArray.resize(nX * nY);
		}
	}
	~Image() {  }

	int xDim = 0, yDim = 0;
	size_t nSamples = 0;
	std::string fileName = "";
	std::vector<float> data;
	std::vector<int> samplesArray;
};


#endif // !IMAGE_H
