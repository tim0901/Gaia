#ifndef SCATTERINGRECORD_h
#define SCATTERINGRECORD_h

#include "Ray.h"

class ScatteringRecord {
public:
	ScatteringRecord() {}

	Ray scatteredRay;
	double scatteringPDF = 0; // PDF from sampling the direction of the new ray
	bool specular = false;
	Vec3d microfacetNormal = Vec3d(0.0, 0.0, 0.0);
};

#endif // !SCATTERINGRECORD_h
