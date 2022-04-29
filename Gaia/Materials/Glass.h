#ifndef GLASS_H
#define GLASS_H

#include "Material.h"
#include "../Maths/Basis.h"

class Glass :public Material {
public:
	Glass(double rI, std::string n) : refractiveIndex(rI) { this->name = n; this->pureSpecular = true; }

	Vec3d EmittedRadiance(const Ray& r, IntersectionRecord& iRec) const {
		return Vec3d(0.0, 0.0, 0.0);
	}

	void SampleDirection(const Ray& r, const IntersectionRecord& iRec, ScatteringRecord& sRec) const;

	Vec3d SampleAndEvaluate(const Ray& r, IntersectionRecord& iRec, ScatteringRecord& sRec) const;

	Vec3d ScatteredRadiance(const Vec3d& inDirection, const Vec3d& outDirection, IntersectionRecord& iRec, ScatteringRecord& sRec) const;

	double EvaluatePDFFromDirections(const Vec3d& in, const Vec3d& scattered, const Vec3d& surfaceNormal) const {
		return 0.0;
	}

	Vec3d R = Vec3d(1.0, 1.0, 1.0);// Vec3d(0.65, 0.05, 0.05);

private:
	double refractiveIndex = 1.0;
};


#endif // !GLASS_H
