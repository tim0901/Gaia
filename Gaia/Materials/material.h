#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Core/IntersectionRecord.h"
#include "../Core/ScatteringRecord.h"
#include "../Maths/Vector.h"
#include "../Maths/Pi.h"
#include "../Maths/Random.h"
#include "Texture.h"

class Material {
public:
	Material() {}

	virtual Vec3d EmittedRadiance(const Ray& r, IntersectionRecord& iRec) const = 0;

	virtual Vec3d SampleAndEvaluate(const Ray& r, IntersectionRecord& iRec, ScatteringRecord& sRec) const = 0;

	virtual void SampleDirection(const Ray& r, const IntersectionRecord& iRec, ScatteringRecord& sRec) const = 0;

	virtual Vec3d ScatteredRadiance(const Vec3d& inDirection, const Vec3d& outDirection, IntersectionRecord& iRec, ScatteringRecord& sRec) const = 0;

	virtual double EvaluatePDFFromDirections(const Vec3d& in, const Vec3d& scattered, const Vec3d& surfaceNormal) const = 0;
	
	Vec3d EvaluateAlphaMap(const IntersectionRecord& iRec) const {

		if (alphaMap) {
			return alphaMap->SampleTexture(iRec);
		}
		else {
			return Vec3d(1.0, 1.0, 1.0);
		}
	}

	bool EvaluateAlphaTransmission(const IntersectionRecord& iRec) const {
		Vec3d alpha = this->EvaluateAlphaMap(iRec);

		if (alpha == Vec3d(1.0, 1.0, 1.0)) {
			// Full alpha - return hit
			return true;
		}
		else if (alpha == Vec3d(0.0, 0.0, 0.0)) {
			return false;
		}
		else {
			// Percentile alpha information

			// Randomly select a channel, then test vs the opacity of that channel
			int channel = (rng.RandomDouble() * 3);
			double opacity = alpha[channel];
			if (rng.RandomDouble() < opacity) {
				return true;
			}
		}
		return false;
	}

	void AddAlphaMap(const std::shared_ptr<Texture> map) {
		alphaMap = map;
	}

	virtual std::string ReturnName() const { return name; }
	std::shared_ptr<Texture> alphaMap;
	std::string name = "";
	bool pureSpecular = false; // For use by purely specular materials eg glass.
};

// Return the reflected vector around a normal vector
// inDirection should be pointing away from the hit point
Vec3d Reflect(const Vec3d& inDirection, const Vec3d& normal);

// Return the refracted vector by a surface
// inDirection should be pointing away from the hit point
bool Refract(const Vec3d& inDirection, const Vec3d& normal, const double& eta, Vec3d& outDirection);

double Fresnel(Vec3d w_i, Vec3d n, double eta_t, double eta_i);

#include "Lambertian.h"
#include "AutodeskSurface.h"
#include "TrowbridgeReitz.h"
#include "Glass.h"

#endif // !MATERIAL_H
