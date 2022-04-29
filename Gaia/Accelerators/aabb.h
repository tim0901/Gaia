#ifndef AABB_H
#define AABB_H

#include "../Maths/Vector.h"
#include "../Core/Ray.h"

class AABB {
public:
	AABB() : minPoint(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX)), maxPoint(Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {}
	AABB(const Vec3f& min, const Vec3f& max) : minPoint(min), maxPoint(max) {}
	
	// Intersection test using Roman Wiche's ray-AABB intersection algorithm
	// Available at https://medium.com/@bromanz/41125138b525
	bool Intersect(const Ray& r, const double& tMin, const double& tMax) const {
		Vec3f invDir = 1.0f / r.direction;
		Vec3f t0 = (minPoint - r.origin) * invDir;
		Vec3f t1 = (maxPoint - r.origin) * invDir;

		Vec3f tSmaller = min(t0, t1);
		Vec3f tBigger = max(t0, t1);

		float min = std::max((float)tMin, std::max(tSmaller[0], std::max(tSmaller[1], tSmaller[2])));
		float max = std::min((float)tMax, std::min(tBigger[0], std::min(tBigger[1], tBigger[2])));

		return (min < max);
	}

	// Return the centre point of the AABB
	Vec3f Centroid() const {
		return 0.5 * (maxPoint + minPoint);
	}

	// Return the position of a given point relative to the dimensions of the box
	Vec3f PositionInside(const Vec3f& v) const {
		Vec3f ret = v - minPoint;

		if (maxPoint.x() > minPoint.x())
			ret[0] /= (maxPoint.x() - minPoint.x());

		if (maxPoint.y() > minPoint.y())
			ret[1] /= (maxPoint.y() - minPoint.y());

		if (maxPoint.z() > minPoint.z())
			ret[2] /= (maxPoint.z() - minPoint.z());
		return ret;
	}



	// Return the surface area of the AABB
	double SurfaceArea() const {
		Vec3f ret = maxPoint - minPoint;
		return 2.0 * (double(ret.x() * ret.y()) + double(ret.y() * ret.z()) + double(ret.z() * ret.z()));
	}

	// Return the enclosed volume
	double Volume() const {
		Vec3f ret = maxPoint - minPoint;
		return double(ret.x() * ret.y() * ret.z());
	}

	// Which side of the AABB is the longest?
	int LongestAxis() const {
		Vec3f diff = maxPoint - minPoint;
		if (diff.x() > diff.y() && diff.x() > diff.z()) {
			return 0;
		}
		else if (diff.y() > diff.z()) {
			return 1;
		}
		else {
			return 2;
		}
	}

	Vec3f minPoint;
	Vec3f maxPoint;
};

inline std::ostream& operator << (std::ostream& os, const AABB& bounds) {
	os << "Min: " << bounds.minPoint << " Max: " << bounds.maxPoint; 
	return os;
}

// Create AABBs
AABB SurroundingBox(const Vec3f& p1, const Vec3f& p2);
AABB SurroundingBox(const AABB& b1, const AABB& b2);
AABB SurroundingBox(const AABB& b1, const Vec3f& p2);

#endif // !AABB_H
