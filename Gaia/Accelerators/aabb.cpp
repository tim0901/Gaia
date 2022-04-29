#include "AABB.h"

// Return the AABB encapsulating two points
AABB SurroundingBox(const Vec3f& p1, const Vec3f& p2) {
	return AABB(Vec3f(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()), std::min(p1.z(), p2.z())),
		Vec3f(std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()), std::max(p1.z(), p2.z())));
}

// Return an AABB surrounding two existing AABBs
AABB SurroundingBox(const AABB& b1, const AABB& b2) {
	return AABB(Vec3f(std::min(b1.minPoint.x(), b2.minPoint.x()), std::min(b1.minPoint.y(), b2.minPoint.y()), std::min(b1.minPoint.z(), b2.minPoint.z())),
		Vec3f(std::max(b1.maxPoint.x(), b2.maxPoint.x()), std::max(b1.maxPoint.y(), b2.maxPoint.y()), std::max(b1.maxPoint.z(), b2.maxPoint.z())));
}

// Extend an AABB to include a point
AABB SurroundingBox(const AABB& b1, const Vec3f& p2) {
	return AABB(Vec3f(std::min(b1.minPoint.x(), p2.x()), std::min(b1.minPoint.y(), p2.y()), std::min(b1.minPoint.z(), p2.z())),
		Vec3f(std::max(b1.maxPoint.x(), p2.x()), std::max(b1.maxPoint.y(), p2.y()), std::max(b1.maxPoint.z(), p2.z())));
}
