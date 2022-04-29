#ifndef TEXTURE_H
#define TEXTURE_H


#ifndef __APPLE__

#pragma warning( push, 0 )
#include "../Dependencies/stb_image.h"
#pragma warning( pop )

#else

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include "../Dependencies/stb_image.h"
#pragma clang diagnostic pop

#endif

#include "../Maths/sRGB.h"
#include "../Maths/Colour.h"
#include "../Core/IntersectionRecord.h"
#include <string>

class Texture {
public:
	Texture() {}

	virtual Vec3d SampleTexture(const IntersectionRecord& iRec) const = 0;
};

class ConstantTexture : public Texture {
public:
	ConstantTexture() {}
	ConstantTexture(const Colour& c) : col(c) {}

	Vec3d SampleTexture(const IntersectionRecord& iRec) const {
		return Vec3d(col.r(), col.g(), col.b());
	}

	Colour col;
};

class ImageTexture : public Texture {
public:
	ImageTexture(const std::string& fileName) {
		pixels = stbi_load(fileName.c_str(), &nx, &ny, &componentsPerPixel, 0);
	}
	~ImageTexture() {
		if (pixels)
			stbi_image_free(pixels);
	}

	Vec3d SampleTexture(const IntersectionRecord& iRec) const {
		// Convert UVs to positions on the image
		int xPos = std::clamp(iRec.uv.u(), 0.0, 1.0) * nx;
		int yPos = (1.0 - std::clamp(iRec.uv.v(), 0.0, 1.0)) * ny; 

		// Ensure positions are within image
		if (xPos >= nx)
			xPos = nx - 1;
		if (yPos >= ny)
			yPos = ny - 1;

		int pixelLocation = ((yPos * nx) + xPos) * componentsPerPixel;
		return InverseSRGBGammaCorrect(Vec3d(pixels[pixelLocation], pixels[pixelLocation + 1], pixels[pixelLocation + 2]) / 255.0);
	}

	int nx = 0, ny = 0, componentsPerPixel = 0;
	stbi_uc* pixels = nullptr;
};
#endif // !TEXTURE_H
