#pragma once
#ifndef SAHBVH_H
#define SAHBVH_H

#include "bvh.h"
#include "memory.h"

class sahBVH : public bvh {
private:
	struct primitiveInfo {
		primitiveInfo(size_t primitiveNumber, const aabb& bounds) : primitiveNumber(primitiveNumber), bounds(bounds), centroid(0.5 * bounds.min() + 0.5 * bounds.max()) {};
		primitiveInfo() {}

		size_t primitiveNumber;
		aabb bounds;
		vec3 centroid;
	};

	struct linearBVHNode {
		aabb bounds;
		union {
			int primitivesOffset;	// If leaf node
			int secondChildOffset;	// If interior node
		};
		uint16_t nPrimitives;		// If 0 then this is an interior node
		uint8_t axis;				// Interior node: xyz
		uint8_t pad[1];				// Pad to 32-bytes for cache alignment
	};

public:
	sahBVH(object** objectList, int numPrimitives, float time0, float time1);
	~sahBVH() {
		//std::cout << "Delete node: ";
		for (int i = 0; i < totalPrimitives; i++) {
			delete primitives[i];
		}
		FreeAligned(nodes);
	}

	int recursiveBuild(object** objectList, std::vector<primitiveInfo>& primInfo, int start, int end, float time0, float time1, int* totalNodes, int* offset, std::vector<object*>& orderedPrimitives);

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;

	float object_id = -2;
	float primitive_id = -2;
	aabb box;
	const int maxPrimsInNode = 255;
	object** primitives;
	int totalPrimitives = 0;
	int numNodes = 0, numInteriorNodes = 0, numLeafNodes = 0;
	linearBVHNode* nodes = nullptr;
	std::vector<object*> orderedPrimitives;
	std::string type = "sahBVH";
};

#endif // !MEDIANBVH_H