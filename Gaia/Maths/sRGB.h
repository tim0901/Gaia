#ifndef SRGB_H
#define SRGB_H

#include <cmath>
import sml;
using namespace sml;

// sRGB -> linear colour correction
double InverseSRGBGammaCorrect(double colour);
Vec3f InverseSRGBGammaCorrect(Vec3f colour);
Vec3d InverseSRGBGammaCorrect(Vec3d colour);

// Linear -> sRGB colour correction
double SRGBGammaCorrect(double colour);
Vec3f SRGBGammaCorrect(Vec3f colour);
Vec3d SRGBGammaCorrect(Vec3d colour);

#endif // !SRGB_H
