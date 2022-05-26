#ifndef BVH_H
#define BVH_H

#include "../Objects/Object.h"
#include "AABB.h"
#include "../Memory/MemoryRegion.h"
#include <thread>
#include <future>
#include <iostream>

struct ObjectInfo {
	ObjectInfo(const AABB& aabb, const size_t& objNumber) : boundingBox(aabb), objectNumber(objNumber), centroid(boundingBox.Centroid()) {}
	AABB boundingBox;
	size_t objectNumber;
	Vec3f centroid;
};

inline std::ostream& operator << (std::ostream& os, const ObjectInfo& info) {
	os << "boundingBox: " << info.boundingBox << "\n";
	os << "objectNumber: " << (int)info.objectNumber << "\n";
	os << "centroid: " << info.centroid;
	return os;
}

struct LinearBVHNode {
	LinearBVHNode() {}
	union {
		int objectOffset;
		int secondChildOffset;
	};
	AABB boundingBox;
	uint16_t nObjects;
	uint8_t axis;
	uint8_t padding[1];
};

inline std::ostream& operator << (std::ostream& os, const LinearBVHNode node) {
	os << "Axis: " << (int)node.axis << "\n";
	os << "nObjects: " << (int)node.nObjects << "\n";
	os << "objectOffset: " << (int)node.objectOffset << "\n";
	os << "secondChildOffset: " << (int)node.secondChildOffset << "\n";
	os << "boundingBox: " << node.boundingBox;
	return os;
}

struct BVHBuildNode {
	BVHBuildNode() {}
	uint8_t axis;
	size_t objectOffset;
	size_t contains;
	std::shared_ptr<BVHBuildNode> childNodes[2];
	AABB boundingBox;
};

inline std::ostream& operator << (std::ostream& os, const BVHBuildNode node) {
	os << "Axis: " << (int)node.axis << "\n";
	os << "objectOffset: " << (int)node.objectOffset << "\n";
	os << "contains: " << (int)node.contains << "\n";
	os << "BoundingBox: " << node.boundingBox;
	return os;
}

class BVH : public Object {
public:
	BVH(std::shared_ptr<std::vector<std::shared_ptr<Object>>> objs) : objects(objs) {
		
		// Check there are actually objects to build with
		if (objects->size() == 0) {
			std::cerr << "Empty bvh" << std::endl;
			return;
		}
		
		std::shared_ptr<BVHBuildNode> rootBuildNode = std::make_shared<BVHBuildNode>();
		orderedObjects = std::make_shared<std::vector<std::shared_ptr<Object>>>(objects->size());

		// Fetch information about all the objects 
		for (int i = 0; i < objects->size(); i++) {
			info.push_back(ObjectInfo(objects->at(i)->BoundingBox(0.0, 1.0), i));
		}

		// Start build process
		std::shared_ptr<std::atomic_int> threadCount = std::make_shared<std::atomic_int>(1);
		rootBuildNode = RecursiveBuild(threadCount, 0, objects->size());

		// Allocate cache-aligned space for the linear bvh nodes
		linearNodes = AllocAligned<LinearBVHNode>(nodeCount);

		// Convert the BVH to a linear BVH for traversal
		int writePointer = 0;
		GenerateLinearBVH(rootBuildNode, &writePointer);

		// We only care about the ordered objects buffer now
		objects.swap(orderedObjects);
		// Empty the now unneeded shared object buffer
		orderedObjects->clear();
		info.clear();

		// Delete the build nodes
		rootBuildNode.reset();
	}
	~BVH() {
		FreeAligned(linearNodes);
	}

	bool Intersect(Ray r, IntersectionRecord& rec, const double tMin = 0.001, const double tMax = DBL_MAX) const;

	void Sample(const Vec3d& p, IntersectionRecord& iRec) const {	}
	void Sample(IntersectionRecord& iRec) const {	}

	double SamplePDF(const Vec3d& p) const { return 0.0; }

	double SurfaceArea() const { return 0.0; }

	AABB BoundingBox(const double t0, const double t1) const { return linearNodes[0].boundingBox; }

private:
	std::shared_ptr<BVHBuildNode> RecursiveBuild(std::shared_ptr<std::atomic_int> threadCount, const int startPos, const int endPos);
	
	int GenerateLinearBVH(std::shared_ptr<BVHBuildNode> node, int* writePointer);

	bool SpatialSplitting(const int& startPos, int& midPoint, const int& endPos, const int& axis, const AABB& centroidBounds);
	bool SAHSplitting(const int& startPos, int& midPoint, const int& endPos, const int& axis, const AABB& centroidBounds);

	bool (BVH::*partitionFunction)(const int&, int&, const int&, const int&, const AABB&) { &BVH::SAHSplitting };

	std::shared_ptr<BVHBuildNode> SpawnLeafNode(const AABB& boundingBox, const int startPos, const int endPos, const int nObjects);

	std::vector<ObjectInfo> info;
	std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects;
	std::shared_ptr<std::vector<std::shared_ptr<Object>>> orderedObjects;
		
	LinearBVHNode* linearNodes = nullptr;
	int maxThreads = 1;//  std::thread::hardware_concurrency(); TODO: multithreaded BVH construction 
	std::atomic_size_t nodeCount = 0;
};



#endif // !BVH_H
