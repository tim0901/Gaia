#pragma once
#ifndef MEDIANBVH_H
#define MEDIANBVH_H

#include "bvh.h"

class medianBVH : public bvh {
public:

	~medianBVH() {
		//std::cout << "Delete node: ";
		if (left && left->type != "triangle") {
			//std::cout << "Delete Left" << left->type << std::endl;
			delete left;
		}
		if (right && right->type != "triangle") {
			//std::cout << "Delete Right" << right->type << std::endl;
			delete right;
		}
	}
	medianBVH(object** objectList, int numPrimitives, float time0, float time1);
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	float object_id = -2;
	float primitive_id = -2;
	object* left;
	object* right;
	aabb box;

	std::string type = "medianBVH";
};

int box_x_compare(const void* a, const void* b);
int box_y_compare(const void* a, const void* b);
int box_z_compare(const void* a, const void* b);



#endif // !MEDIANBVH_H


