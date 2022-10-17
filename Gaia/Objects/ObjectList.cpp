#include "ObjectList.h"

// Does the ray intersect any of these objects?
bool ObjectList::Intersect(Ray r, IntersectionRecord& rec, const double tMin, const double tMax) const {
	double closestT = tMax;
	double minT = tMin;
	bool hitSomething = false;
	IntersectionRecord tempIntersection;

	// Find the closest object
	for (size_t i = 0; i < list->size(); i++) {
		if (list->at(i)->Intersect(r, tempIntersection, minT, closestT)) {
			hitSomething = true;
			if (tempIntersection.t < closestT) {
				closestT = tempIntersection.t;
				rec = tempIntersection;
			}

		}
	}
	return hitSomething;
}