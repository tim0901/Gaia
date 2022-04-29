#ifndef OBJECT_H
#define OBJECT_H

#include "../Core/Ray.h"
#include "../Core/IntersectionRecord.h"
#include "../Accelerators/AABB.h"

class Light;

class Object {
public:
	Object() {}

	// Perform ray-object intersection test 
	virtual bool Intersect(Ray r, IntersectionRecord& rec, const double tMin = 0.001, const double tMax = DBL_MAX) const = 0;

	// Sample a point on the surface of the object from a point being illuminated by it
	virtual void Sample(const Vec3d& p, IntersectionRecord& iRec) const = 0;
	virtual void Sample(IntersectionRecord& iRec) const = 0;

	Light* ReturnLight() const { return light; };
	virtual void AttachLight(Light* l) { light = l; }

	virtual double SamplePDF(const Vec3d& p) const = 0;
	virtual double SurfaceArea() const = 0;

	virtual AABB BoundingBox(const double t0, const double t1) const = 0;

	Material* material = nullptr;
	Light* light = nullptr;
};

#include "ObjectList.h"
#include "Sphere.h"
#include "Triangle.h"

#endif // !OBJECT_H