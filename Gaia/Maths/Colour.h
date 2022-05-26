#pragma once
import sml;
using namespace sml;
#include "sRGB.h"

// Define a colour with linear values
class Colour :
    public Vector<double, 3>
{
public:
    Colour() {}
    Colour(double r, double g, double b) {
        (*this)[0] = r; 
        (*this)[1] = g; 
        (*this)[2] = b; 
    }
};

// Define a colour with sRGB values, which are stored as a linear colour for internal use
class sRGBColour : public Colour {
public:
    sRGBColour(double r, double g, double b) {
        (*this)[0] = InverseSRGBGammaCorrect(r);
        (*this)[1] = InverseSRGBGammaCorrect(g);
        (*this)[2] = InverseSRGBGammaCorrect(b);
    }
    sRGBColour(Vec3d col) {
        (*this) = InverseSRGBGammaCorrect(col);
    }
};