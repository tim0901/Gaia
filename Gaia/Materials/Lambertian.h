#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "Material.h"
#include "../Maths/Colour.h"

class Lambertian : public Material
{
public:
	Lambertian(std::shared_ptr<Texture> t, std::string n) : reflectance(t) { this->name = n; }
	Lambertian(const Colour& c, std::string n) : reflectance(std::make_shared<ConstantTexture>(c)) { this->name = n; }

	Vec3d EmittedRadiance(const Ray& r, IntersectionRecord& iRec) const { return Vec3d(0, 0, 0); }

	Vec3d SampleAndEvaluate(const Ray& r, IntersectionRecord& iRec, ScatteringRecord& sRec) const {

		SampleDirection(r, iRec, sRec); 
		return ScatteredRadiance(r.direction, sRec.scatteredRay.direction, iRec, sRec);
		return Vec3d(0.0, 0.0, 0.0);
	}


	double EvaluatePDFFromDirections(const Vec3d& in, const Vec3d& scattered, const Vec3d& surfaceNormal) const {
		if (dot(in, scattered) > 0) {
			return rng.RandomCosineHemispherePDF(scattered, surfaceNormal);
		}
		return 0.0;
	}

	// Generate a new ray direction based on the reflective properties of the surface
	void SampleDirection(const Ray& r, const IntersectionRecord& iRec, ScatteringRecord& sRec) const {
		
		// A newly sampled direction in local coordinates
		Vec3d newDirection = rng.RandomCosineHemisphere();
		
		if (dot(iRec.normal, newDirection) < 0.0) {
			// Wrong hemisphere
			newDirection *= -1;
		}

		sRec.scatteredRay = Ray(iRec.p, unit_vector(newDirection));
		sRec.scatteringPDF = EvaluatePDFFromDirections(r.direction, sRec.scatteredRay.direction, iRec.normal);
	}

	Vec3d ScatteredRadiance(const Vec3d& inDirection, const Vec3d& outDirection, IntersectionRecord& iRec, ScatteringRecord& sRec) const {
		return reflectance->SampleTexture(iRec) * M_1_PI;
	}

	std::shared_ptr<Texture> reflectance; // rho
};

#endif // !LAMBERTIAN_H