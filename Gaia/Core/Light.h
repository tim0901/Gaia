#ifndef LIGHT_H
#define LIGHT_H

#include "../Objects/Object.h"

class Light {
public:
	Light() {}

	virtual Vec3d EmittedRadiance(const Ray& r, IntersectionRecord& iRec) const = 0;

	virtual Vec3d Le(const Ray& r, const IntersectionRecord& iRec) const = 0;

	virtual Vec3d Sample(const Vec3d& p, Vec3d& sampledDirection, double& pdf, bool visibility) const = 0;

	// Return probability density of sampling direction d from point p
	virtual double EvaluatePdfInDirection(const Vec3d& p, const Vec3d& d) const = 0;

	std::shared_ptr<Object> ReturnAttachedObject() const { return object; }

	std::shared_ptr<Object> object = nullptr;
};


class AreaLight : public Light {
public:
	AreaLight(std::shared_ptr<Object> o, Vec3d i) : intensity(i) { this->object = o; }

	Vec3d EmittedRadiance(const Ray& r, IntersectionRecord& iRec) const {
		return Vec3d(0.0, 0.0, 0.0);
	}

	Vec3d Le(const Ray& r, const IntersectionRecord& iRec) const {
		return dot(iRec.normal, -unit_vector(r.direction)) > 0.0 ? intensity : Vec3d(0.0, 0.0, 0.0);
	}

	Vec3d Sample(const Vec3d& p, Vec3d& sampledDirection, double& pdf, bool visibility) const {

		IntersectionRecord iRec;
		object->Sample(p, iRec);
		sampledDirection = iRec.p - p;
		pdf = object->SamplePDF(p);
		Ray r(p, unit_vector(sampledDirection));

		visibility = object->Intersect(r, iRec);
		return dot(iRec.normal, -unit_vector(sampledDirection)) > 0.0 ? intensity : Vec3d(0.0, 0.0, 0.0);
	};

	double EvaluatePdfInDirection(const Vec3d& p, const Vec3d& d) const {

		return object->SamplePDF(p);
	}

	Vec3d intensity;
};

#endif // !LIGHT_H
