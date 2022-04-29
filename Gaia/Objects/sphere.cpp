#include "Sphere.h"

bool Sphere::Intersect(Ray r, IntersectionRecord& iRec, const double tMin, const double tMax) const {

	Vec3d oc = r.origin - centre;

	double a = dot(r.direction, r.direction);
	double b = dot(oc, r.direction);
	double c = dot(oc, oc) - radius * radius;
	double discriminant = b * b - a * c;

	if (discriminant > 0.0) {
		double temp = (-b - sqrt(discriminant)) / a;
		if (temp < tMax && temp > tMin) {
			iRec.light = this->light;
			iRec.t = temp;
			iRec.p = r.PositionAtt(iRec.t);
			iRec.normal = unit_vector((iRec.p - centre) / radius);
			iRec.mat = material;
			iRec.uv = CalculateSphereUVs((iRec.p - centre) / radius);

			// Evaluate the alpha map to see if this section of the sphere is to be rendered
			if (iRec.mat->EvaluateAlphaTransmission(iRec))
				return true;

		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < tMax && temp > tMin) {
			iRec.light = this->light;
			iRec.t = temp;
			iRec.p = r.PositionAtt(iRec.t);
			iRec.normal = unit_vector((iRec.p - centre) / radius);
			iRec.mat = material;
			iRec.uv = CalculateSphereUVs((iRec.p - centre) / radius);

			// Evaluate the alpha map to see if this section of the sphere is to be rendered
			if (iRec.mat->EvaluateAlphaTransmission(iRec))
				return true;
		}
	}

	return false;
}

void Sphere::Sample(const Vec3d& p, IntersectionRecord& rec) const {
		
	// Basis vectors for the sphere with Z-axis in the direction of the sampled point
	Basis basis = BasisFromVector(unit_vector(centre - p));

	// Are we inside the sphere?
	if (squared_length(centre-rec.p) <= radius * radius) {
		// YES! Uniformally sample the sphere as it's visible in all directions
		std::cout << "Ye be inside me sphere... TODO!" << std::endl;
		return Sample(rec);
	}

	double sinThetaMax = radius / length(p - centre);
	double sinThetaMax2 = sinThetaMax * sinThetaMax;
	double invSinThetaMax = 1 / sinThetaMax;
	double cosThetaMax = sqrt(std::max(0.0, 1.0 - sinThetaMax2));

	double r1 = rng.RandomDouble(), r2 = rng.RandomDouble();

	double cosTheta = (cosThetaMax - 1.0) * r1 + 1.0;
	double sinTheta2 = 1.0 - cosTheta * cosTheta;

	if (sinThetaMax2 < 0.00068523) {
		// Use taylor series expansion as standard approach has serious errors
		sinTheta2 = sinThetaMax2 * r1;
		cosTheta = std::sqrt(1.0 - sinTheta2);
	}

	double cosAlpha = sinTheta2 * invSinThetaMax + cosTheta * std::sqrt(std::max(0.0, 1.0 - sinTheta2 * invSinThetaMax * invSinThetaMax));
	double sinAlpha = sqrt(std::max(0.0, 1.0 - cosAlpha * cosAlpha));
	double phi = r2 * M_2PI;

	Vec3d normal = sinAlpha * cos(phi) * (-basis.v) + sinAlpha * sin(phi) * (-basis.w) + cosAlpha * (-basis.u);
	//Vec3d pObj = radius * radius * normal / length(p);
	Vec3d pWorld = centre + radius * normal;

	/*

	// Given a sample angle (theta, phi) wrt our coordinate system, we can create a sample on the sphere
	double dc = length(centre - p);
	double ds = dc * cosTheta - sqrt(std::max(0.0, radius * radius - dc * dc * sinTheta * sinTheta));
	double cosAlpha = (dc * dc + radius * radius - ds * ds) / (2.0 * dc * radius);
	*/
	
	//std::cout << "Sampled position on sphere: " << pWorld << std::endl;

	rec.normal = unit_vector(normal);
	rec.p = pWorld;
}

void Sphere::Sample(IntersectionRecord& rec) const {


	//return Vec3d(0.0, 0.0, 0.0);
}

double Sphere::SamplePDF() const {
	// Simple 1 / Area
	return 1 / SurfaceArea();
}

double Sphere::SamplePDF(const Vec3d& p) const {

	// Are we inside the sphere?
	if (squared_length(centre - p) <= radius * radius) {
		// YES! Return uniform pdf
		return SamplePDF();
	}
	double sinThetaMax2 = radius * radius / squared_length(p - centre);
	double cosThetaMax = sqrt(std::max(0.0, 1 - sinThetaMax2));
	return 1.0 / (M_2PI * (1 - cosThetaMax));
}