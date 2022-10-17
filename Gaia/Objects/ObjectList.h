#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include "Object.h"
#include <vector>

class ObjectList : public Object {
public:
	ObjectList() {
		list = std::make_shared<std::vector<std::shared_ptr<Object>>>();
	}

	bool Intersect(Ray r, IntersectionRecord& rec, const double tMin = 0.001, const double tMax = DBL_MAX) const;

	void Sample(const Vec3d& p, IntersectionRecord& iRec) const {};
	void Sample(IntersectionRecord& iRec) const {};

	double SamplePDF(const Vec3d& p) const { return 0.0; };

	AABB BoundingBox(const double t0, const double t1) const {
		AABB ret;
		for (int i = 0; i < list->size(); i++) {
			ret = SurroundingBox(ret, list->at(i)->BoundingBox(t0, t1));
		}
		return ret;
	}

	// Insert an object into the list
	template<typename T>
	void InsertObject(const T& obj) {
		list->push_back(std::make_shared<T>(obj));
	}
	template<typename T>
	void InsertObject(const std::shared_ptr<T>& obj) {
		list->push_back(obj);
	}

	void AttachLight(Light* l) {
		for (std::shared_ptr<Object> obj : *list) {
			obj->AttachLight(l);
		}
		this->light = l;
	}

	size_t size() const {
		return list->size();
	}

	double SurfaceArea() const {
		double totalArea = 0.0;
		for (std::shared_ptr<Object> obj : *list) {
			totalArea += obj->SurfaceArea();
		}
		return totalArea;
	}

	std::shared_ptr<std::vector<std::shared_ptr<Object>>> list;
};


#endif // !OBJECTLIST_H
