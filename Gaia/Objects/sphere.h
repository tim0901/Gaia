#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"
#include "../Materials/Material.h"
#include "../Maths/Basis.h"

class Sphere : public Object {
public:
	Sphere(Vec3d c, double r, Material* mat) : centre(c), radius(r) { this->material = mat; }

	bool Intersect(Ray r, IntersectionRecord& iRec, const double tMin = 0.001, const double tMax = DBL_MAX) const;

	void Sample(const Vec3d& p, IntersectionRecord& iRec) const;
	void Sample(IntersectionRecord& iRec) const;

	double SamplePDF(const Vec3d& p) const;
	double SamplePDF() const;

	double SurfaceArea() const { return 4 * M_PI * radius * radius; }

	AABB BoundingBox(const double t0, const double t1) const {
		return AABB(centre - radius, centre + radius);
	}

	Vec3d centre;
	double radius;

private:
	Vec2d CalculateSphereUVs(const Vec3d& p) const {
		double phi = atan2(p.z(), p.x());
		double theta = asin(p.y());
		
		return Vec2d(1 - (phi + M_PI) * (M_1_PI * 0.5), (theta + M_PI_2) * M_1_PI);
	}
};


#endif // !OBJECTLIST_H
