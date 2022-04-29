#include "sRGB.h"

// sRGB -> linear colour correction
double InverseSRGBGammaCorrect(double colour) {
	if (colour <= 0.04045) {
		return colour * 1.0 / 12.92;
	}
	else {
		return std::pow((colour + 0.055) * 1.0 / 1.055, (double)2.4);
	}
}

Vec3f InverseSRGBGammaCorrect(Vec3f colour) {
	return Vec3f(InverseSRGBGammaCorrect(colour.r()), InverseSRGBGammaCorrect(colour.g()), InverseSRGBGammaCorrect(colour.b()));
}
Vec3d InverseSRGBGammaCorrect(Vec3d colour) {
	return Vec3d(InverseSRGBGammaCorrect(colour.r()), InverseSRGBGammaCorrect(colour.g()), InverseSRGBGammaCorrect(colour.b()));
}

// Linear -> sRGB colour correction
double SRGBGammaCorrect(double colour) {
	if (colour <= 0.0031308) {
		return colour * 12.92;
	}
	else {
		return 1.055 * std::pow(colour, (double)(1.0 / 2.4)) - 0.055;
	}
}

Vec3f SRGBGammaCorrect(Vec3f colour) {
	return Vec3f(SRGBGammaCorrect(colour.r()), SRGBGammaCorrect(colour.g()), SRGBGammaCorrect(colour.b()));
}
Vec3d SRGBGammaCorrect(Vec3d colour) {
	return Vec3d(SRGBGammaCorrect(colour.r()), SRGBGammaCorrect(colour.g()), SRGBGammaCorrect(colour.b()));
}
