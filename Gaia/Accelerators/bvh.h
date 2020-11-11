#pragma once

#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "random.h"
#include "object.h"

enum {
	MEDIAN = 0,
	SAH = 1,
	SBVH = 2
};

class bvh : public object {
public:
	//bvh(object **objectList, int numPrimitives, float time0, float time1, int type = MEDIAN ) {};

	static bvh* constructBVH(object** objectList, int numPrimitives, float time0, float time1, int type);

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
	float object_id = -2;
	float primitive_id = -2;
	object* left;
	object* right;
	aabb box;

	std::string type = "bvh";

protected:
	bvh() {}
	~bvh() {}
};
#endif //BVH_H
