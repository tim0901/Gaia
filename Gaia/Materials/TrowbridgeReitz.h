#ifndef TROWBRIDGE_REITZ_H
#define TROWBRIDGE_REITZ_H

#include "Material.h"

// Signum function
// Returns -1 for negative numbers, +1 for positive numbers and 0 for 0
template<typename T>
T Signum(const T& t) {
	return (T(0) < t) - (t < T(0));
}

// Positive Characteristic function
// Returns 1 for positive numbers and 0 otherwise
template<typename T>
T PositiveCharacteristicFunction(T t) {
	return static_cast<T>(t > 0.0);
}

class TrowbridgeReitz : public Material {
public:
	TrowbridgeReitz(std::shared_ptr<Texture> t, double rI, double alpha, std::string n) : reflectance(t), alphaG(alpha), refractiveIndex(rI) { this->name = n; }

	Vec3d EmittedRadiance(const Ray& r, IntersectionRecord& iRec) const {
		return Vec3d(0.0, 0.0, 0.0);
	}

	void SampleDirection(const Ray& r, const IntersectionRecord& iRec, ScatteringRecord& sRec) const;

	Vec3d SampleAndEvaluate(const Ray& r, IntersectionRecord& iRec, ScatteringRecord& sRec) const;

	Vec3d ScatteredRadiance(const Vec3d& inDirection, const Vec3d& outDirection, IntersectionRecord& iRec, ScatteringRecord& sRec) const;

	double EvaluatePDFFromDirections(const Vec3d& in, const Vec3d& scattered, const Vec3d& surfaceNormal) const {
		return D(unit_vector(in
			+ scattered), surfaceNormal) * abs(dot(unit_vector(in + scattered), surfaceNormal));
	}

	double alphaG = 0.001;
	double refractiveIndex = 1.0;
	std::shared_ptr<Texture> reflectance;
private:

	double D(Vec3d m, Vec3d n) const {

		//std::cout << "SURFACE NORMAL: " << n << " " << length(n) << std::endl;
		//std::cout << "MICROSURFACE NORMAL: " << m << " " << length(m) << std::endl;

		double alphaGSq = alphaG * alphaG;
		double cosTheta = dot(m, n);
		double tanTheta = tan(acos(cosTheta));

		//std::cout << "alphaGSq: "<<alphaGSq << std::endl;
		//std::cout << "cosTheta: "<<cosTheta << std::endl;
		//std::cout << "tanTheta: "<<tanTheta<< std::endl;
		//std::cout << "posCharFunc: "<<PositiveCharacteristicFunction(cosTheta) << std::endl;

		double ret = (alphaGSq * PositiveCharacteristicFunction(cosTheta)) / (M_PI * pow(cosTheta, 4.0) * pow((alphaGSq + (tanTheta * tanTheta)), 2.0));
		//std::cout << "D: " << ret << std::endl;
		return ret;
	}
	double G(Vec3d w_i, Vec3d w_o, Vec3d m, Vec3d n) const {
		//std::cout << "G1s: " <<G1(w_i, m, n) << " " << G1(w_o, m, n) << std::endl;
		return G1(w_i, m, n) * G1(w_o, m, n);
	}

	double G1(Vec3d w, Vec3d m, Vec3d n) const {
		double cosTheta = dot(w, n);
		double tanTheta = tan(acos(cosTheta));

		double tan2Theta = tanTheta * tanTheta;
		//std::cout << "NEXT" << std::endl;
		//std::cout << PositiveCharacteristicFunction(dot(w, m) / cosTheta) << std::endl;
		//std::cout << w << " " << m << " " << n << std::endl;
		//std::cout << dot(w, m) / cosTheta << std::endl;
		//std::cout << "CosTheta: " << cosTheta << " " << tanTheta << " " << tan2Theta << std::endl;
		//std::cout << sqrt(1.0 + (alphaG * alphaG * tan2Theta)) << std::endl;

		return PositiveCharacteristicFunction(dot(w, m) / cosTheta) * (2.0 / (1.0 + sqrt(1.0 + (alphaG * alphaG * tan2Theta))));
	}
};

#endif //TROWBRIDGE_REITZ_H