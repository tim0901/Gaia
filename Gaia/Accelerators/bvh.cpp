#include "BVH.h"

std::shared_ptr<BVHBuildNode> BVH::RecursiveBuild(std::shared_ptr<std::atomic_int> threadCount, const int startPos, const int endPos) {

	// 1. Create a bounding box for the entire node
	AABB nodeBounds;
	for (int i = startPos; i < endPos; i++) {
		nodeBounds = SurroundingBox(nodeBounds, info[i].boundingBox);
	}

	int nObjects = endPos - startPos;
	if (nObjects == 1) {
		// Make a leaf node
		return SpawnLeafNode(nodeBounds, startPos, endPos, nObjects);
	}
	else {
		// Intermediary node

		// Create a bounding box of the centroids
		AABB centroidBounds;
		for (int i = startPos; i < endPos; i++) {
			centroidBounds = SurroundingBox(centroidBounds, info[i].centroid);
		}

		// Choose which axis to split along
		int axis = centroidBounds.LongestAxis();

		int midPoint = (startPos + endPos) * 0.5;

		if (centroidBounds.maxPoint[axis] == centroidBounds.minPoint[axis]) {
			// All the centroids are in the same place - spawn a leaf node
			return SpawnLeafNode(nodeBounds, startPos, endPos, nObjects);
		}
		else {
			
			// Sorting time!

			// Call the partition function
			bool split = (this->*partitionFunction)(startPos, midPoint, endPos, axis, centroidBounds);

			if (!split) {
				// A split was not considered worthwhile, so we spawn a leaf node
				return SpawnLeafNode(nodeBounds, startPos, endPos, nObjects);

			}
			else {
				// Split!
				
				// Create a node
				std::shared_ptr<BVHBuildNode> node = std::make_shared<BVHBuildNode>();
				nodeCount++;
				
				// Fill in the node
				node->boundingBox = nodeBounds;
				node->contains = 0;
				node->axis = axis;

				// Branch
				if (*threadCount < maxThreads) {
					// Spawn another thread!
					(*threadCount)++;

					std::cout << "Spawning another thread\n";

					// Spawn secondary thread exploring one subpath
					std::future<std::shared_ptr<BVHBuildNode>> future = std::async([this, threadCount, startPos, midPoint] {return this->RecursiveBuild(threadCount, startPos, midPoint); });

					// Continue to explore the other subpath in this thread
					node->childNodes[1] = RecursiveBuild(threadCount, midPoint, endPos);

					// Collect the future from the secondary thread
					node->childNodes[0] = future.get();
					(*threadCount)--;
				}
				else {
					// Explore both branches in this thread
					node->childNodes[0] = RecursiveBuild(threadCount, startPos, midPoint);
					node->childNodes[1] = RecursiveBuild(threadCount, midPoint, endPos);
				}

				// Create a bounding box for this node from those of the two child nodes
				node->boundingBox = SurroundingBox(node->childNodes[0]->boundingBox, node->childNodes[1]->boundingBox);
				return node;
			}
			
		}

	}
}

// Spawn a leaf node!
std::shared_ptr<BVHBuildNode> BVH::SpawnLeafNode(const AABB& boundingBox, const int startPos, const int endPos, const int nObjects) {

	// Create a node!
	std::shared_ptr<BVHBuildNode> node = std::make_shared<BVHBuildNode>();
	nodeCount++;

	node->boundingBox = boundingBox;

	// Add these objects to the "sorted" pile
	int firstObjectOffset = orderedObjects->size();
	for (int i = startPos; i < endPos; ++i) {
		orderedObjects->push_back(objects->at(info[i].objectNumber));
	}

	// Fill in the rest of the node and return
	node->objectOffset = firstObjectOffset;
	node->childNodes[0] = nullptr;
	node->childNodes[1] = nullptr;
	node->contains = nObjects;
	return node;
}

// Convert the BVHBuildNode stack into a linear BVH that is quicker to traverse
int BVH::GenerateLinearBVH(std::shared_ptr<BVHBuildNode> buildNode, int* writePointer) {

	// Find the correct position in the array
	LinearBVHNode* linearNode = &linearNodes[*writePointer];
	linearNode->boundingBox = buildNode->boundingBox;
	
	int thisNode = (*writePointer)++;
	if (buildNode->contains > 0) {
		// This is a leaf node
		linearNode->objectOffset = buildNode->objectOffset;
		linearNode->nObjects = buildNode->contains;
	}
	else {
		// This is an interior node
		linearNode->axis = buildNode->axis;
		linearNode->nObjects = 0;
		GenerateLinearBVH(buildNode->childNodes[0], writePointer);
		linearNode->secondChildOffset = GenerateLinearBVH(buildNode->childNodes[1], writePointer);
	}
	return thisNode;
}

bool BVH::Intersect(Ray r, IntersectionRecord& rec, const double tMin, const double tMax) const {
	
	int nodesToVisit[64] = {0};
	int toVisitOffset = 0, currentNodeIndex = 0;

	Vec3f invDirection = 1.0f / r.direction;
	int directionIsNegative[3] = {static_cast<int>(invDirection.x() < 0), static_cast<int>(invDirection.y() < 0), static_cast<int>(invDirection.z() < 0)};

	IntersectionRecord tempRecord;
	bool hit = false;
	double closestSoFar = tMax;

	while (true) {
		if (currentNodeIndex < 0)
			return false;
		
		// Obtain next node to be traversed
		const LinearBVHNode* linearNode = &linearNodes[currentNodeIndex];		

		if (linearNode->boundingBox.Intersect(r, tMin, closestSoFar)) {
			if (linearNode->nObjects > 0) {
				// Root node!				
				// Perform intersection test with the objects contained by this node
				for (int i = 0; i < linearNode->nObjects; ++i) {

					if (objects->at(linearNode->objectOffset + i)->Intersect(r, tempRecord, tMin, closestSoFar)) {
						hit = true;
						if (tempRecord.t < closestSoFar) {
							closestSoFar = tempRecord.t;
							rec = tempRecord;
						}
					}
				}

				// All nodes in the stack must be visited, even if we have a primitive hit. 
				// If the stack is empty, then we may break.

				if (toVisitOffset == 0)
					break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else {
				// Interior node!

				// Interior node
				// Add further bvh node on the to do list, advance to the nearer node
				if (directionIsNegative[linearNode->axis]) {
					// Use the second child
					nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
					currentNodeIndex = linearNode->secondChildOffset;
				}
				else {
					nodesToVisit[toVisitOffset++] = linearNode->secondChildOffset;
					currentNodeIndex = currentNodeIndex + 1;
				}
			}
		}
		else {
			// Miss
			if (toVisitOffset == 0)
				break;
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	return hit;
}

bool BVH::SpatialSplitting(const int& startPos, int& midPoint, const int& endPos, const int& axis, const AABB& centroidBounds) {
	// Split the objects into equally sized subsets
	std::nth_element(&info[startPos], &info[midPoint], &info[endPos - 1] + 1,
		[axis](const ObjectInfo& a, const ObjectInfo& b) {
			return a.centroid[axis] < b.centroid[axis];
		});
	return true;
}

struct SAHBin {
	SAHBin() {};
	SAHBin(int n, AABB b) : nObjects(n), boundingBox(b) {};
	int nObjects = 0;
	AABB boundingBox;
};

bool BVH::SAHSplitting(const int& startPos, int& midPoint, const int& endPos, const int& axis, const AABB& centroidBounds) {

	const int nObjects = endPos - startPos;

	// It isn't worth calculating SAH at this point - just perform spatial splitting
	if (nObjects <= 4)
		return SpatialSplitting(startPos, midPoint, endPos, axis, centroidBounds);

	// Create 12 bins to sort objects into 
	constexpr int nBins = 12;
	SAHBin bins[nBins];

	// Populate bins
	for (int i = startPos; i < endPos; i++) {

		// Find which bin this object fits into
		int bin = nBins * centroidBounds.PositionInside(info[i].centroid)[axis];

		if (bin == nBins) 
			bin = nBins - 1;

		// Expand the bounding box of this bin to include this point
		bins[bin].boundingBox = SurroundingBox(bins[bin].boundingBox, info[i].centroid);
		bins[bin].nObjects++;
	}

	AABB left;
	int leftObjects = 0;
	int rightObjects = nObjects;

	int bestBin = 0; // The index of the bin to the left of the split. This can therefore go from 0-10 as we must have a bin on the rhs in order to perform a split
	float bestCost = FLT_MAX;

	float inverseBoundsSurfaceArea = 1.0 / centroidBounds.SurfaceArea();

	// Working from left -> right
	for (int i = 0; i < nBins - 1; i++) {

		// Expand left bin to include the new 
		left = SurroundingBox(left, bins[i].boundingBox);

		// Adjust the number of objects in each bin
		leftObjects += bins[i].nObjects;
		rightObjects -= bins[i].nObjects;

		// Recalculate the size of right bin
		AABB right;
		for (int j = nBins - 1; j > i; j--) {
			right = SurroundingBox(right, bins[j].boundingBox);
		}

		// Calculate the cost
		float cost = 0.125f + (leftObjects * left.SurfaceArea() + rightObjects * right.SurfaceArea()) * inverseBoundsSurfaceArea;
		if (cost < bestCost) {
			bestCost = cost;
			bestBin = i;
		}
	}

	int maxObjectsInNode = 255;
	float leafNodeCost = nObjects;
	if (leafNodeCost < bestCost && nObjects < maxObjectsInNode) {
		// Splitting here isn't worthwhile - just make a leaf node
		return false;
	}

	// Find the object in the ObjectInfo array that corresponds to the splitting point
	ObjectInfo* midObject = std::partition(&info[startPos], &info[endPos - 1] + 1,
		[=](const ObjectInfo& obj) {
			int b = nBins * centroidBounds.PositionInside(obj.centroid)[axis];
			if (b == nBins)
				b = nBins - 1;
			return b <= bestBin;
		});
	midPoint = midObject - &info[0];
	return true;
}