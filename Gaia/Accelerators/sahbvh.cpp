#include "sahbvh.h"

int sahBVH::recursiveBuild(object** objectList, std::vector<primitiveInfo>& primInfo, int start, int end, float time0, float time1, int* totalNodes, int* offset, std::vector<object*>& orderedPrimitives) {

	(*totalNodes)++; // Increment total node count

	// Compute bounds of all prims in node
	aabb bounds;
	for (int i = start; i < end; ++i) {
		bounds = surrounding_box(bounds, primInfo[i].bounds);
	}
	
	int nPrimitives = end - start;
	if (nPrimitives == totalPrimitives) { // Store bounding box for entire mesh
		box = bounds;
	}

	if (nPrimitives == 1) {
		// Create leaf node
		int firstPrimOffset = orderedPrimitives.size();
		for (int i = start; i < end; ++i) {
			int primNum = primInfo[i].primitiveNumber;
			orderedPrimitives.push_back(objectList[primNum]);
		}

		// Create linear leaf node
		numLeafNodes++;
		linearBVHNode* linearNode = &nodes[*offset];
		linearNode->bounds = bounds;
		int myOffset = (*offset)++;
		linearNode->primitivesOffset = firstPrimOffset;
		linearNode->nPrimitives = nPrimitives;
		
		return myOffset;
	}
	else {
		// Compute bounds of prim centroids, use to choose which axis to split along
		aabb centroidBounds;

		for (int i = start; i < end; ++i) {
			centroidBounds = surrounding_box(centroidBounds, primInfo[i].centroid);
		}

		int axis = centroidBounds.longestAxis();

		// Partition prims into two sets and build children.
		int mid = (start + end) / 2;
		if (centroidBounds.max()[axis] == centroidBounds.min()[axis]) {
			// If all the centroids are in the same place, make a leaf node

			int firstPrimOffset = orderedPrimitives.size();
			for (int i = start; i < end; ++i) {
				int primNum = primInfo[i].primitiveNumber;
				orderedPrimitives.push_back(objectList[primNum]);
			}

			// Create linear leaf node
			numLeafNodes++;
			linearBVHNode* linearNode = &nodes[*offset];
			linearNode->bounds = bounds;
			int myOffset = (*offset)++;
			linearNode->primitivesOffset = firstPrimOffset;
			linearNode->nPrimitives = nPrimitives;

			return myOffset;

		}
		else {
			// Partition time

			if (nPrimitives <= 4) {
				// Partition equally - SAH isn't worth it here
				mid = (start + end) * 0.5;
				std::nth_element(&primInfo[start], &primInfo[mid], &primInfo[end - 1] + 1, [axis](const primitiveInfo& a, const primitiveInfo& b) {
					return a.centroid[axis] < b.centroid[axis];
					});
			}
			else {
				// Partition according to SAH
				constexpr int nBuckets = 12;
				struct BucketInfo {
					int count = 0;
					aabb bounds;
				};
				BucketInfo buckets[nBuckets];

				// Init bucketinfo
				for (int i = start; i < end; ++i) {
					int b = nBuckets * centroidBounds.positionInside(primInfo[i].centroid)[axis];
					if (b == nBuckets) {
						b = nBuckets - 1;
					}
					buckets[b].count++;
					buckets[b].bounds = surrounding_box(buckets[b].bounds, primInfo[i].bounds);
				}

				// Compute cost for each bucket
				float cost[nBuckets - 1];
				for (int i = 0; i < nBuckets - 1; ++i) {
					aabb b0, b1;
					double count0 = 0, count1 = 0;
					for (int j = 0; j <= i; ++j) {
						b0 = surrounding_box(b0, buckets[j].bounds);
						count0 += buckets[j].count;
					}
					for (int j = i + 1; j < nBuckets; ++j) {
						b1 = surrounding_box(b1, buckets[j].bounds);
						count1 += buckets[j].count;
					}

					// We make the (pretty accurate) assumption here that all primitive intersection tests take 8x longer 
					// than an aabb intersection test. 
					cost[i] = 0.125 + ((count0 * b0.surfaceArea() + count1 * b1.surfaceArea()) / bounds.surfaceArea());
				}

				// Find cheapest bucket to split at 
				float minCost = cost[0];
				int minCostSplitBucket = 0;
				for (int i = 1; i < nBuckets - 1; ++i) {
					if (cost[i] < minCost) {
						minCost = cost[i];
						minCostSplitBucket = i;
					}
				}

				float leafCost = nPrimitives;
				if (nPrimitives > maxPrimsInNode || minCost < leafCost) {
					// Split prims at selected SAH bucket
					primitiveInfo* pmid = std::partition(&primInfo[start], &primInfo[end - 1] + 1, [=](const primitiveInfo& pi) {
						int b = nBuckets * centroidBounds.positionInside(pi.centroid)[axis];

						if (b == nBuckets) {
							b = nBuckets - 1;
						}
						return b <= minCostSplitBucket;
						});
					mid = pmid - &primInfo[0];
				}
				else {
					// Create leaf

					int firstPrimOffset = orderedPrimitives.size();
					for (int i = start; i < end; ++i) {
						int primNum = primInfo[i].primitiveNumber;
						orderedPrimitives.push_back(objectList[primNum]);
					}

					// Create linear leaf node
					numLeafNodes++;
					linearBVHNode* linearNode = &nodes[*offset];
					linearNode->bounds = bounds;
					int myOffset = (*offset)++;
					linearNode->primitivesOffset = firstPrimOffset;
					linearNode->nPrimitives = nPrimitives;

					return myOffset;
				}
			}

			// Create interior node

			// Create linear interior node
			numInteriorNodes++;
			linearBVHNode* linearNode = &nodes[*offset];
			linearNode->bounds = bounds;
			int myOffset = (*offset)++;
			linearNode->axis = axis;
			linearNode->nPrimitives = 0;
			recursiveBuild(objectList, primInfo, start, mid, time0, time1, totalNodes, offset, orderedPrimitives); // The first node is created directly after this one as desired;
			linearNode->secondChildOffset = recursiveBuild(objectList, primInfo, mid, end, time0, time1, totalNodes, offset, orderedPrimitives);

			return myOffset;
		}
	}
}

sahBVH::sahBVH(object** objectList, int numPrimitives, float time0, float time1) {

	primitives = objectList;
	totalPrimitives = numPrimitives;

	/// Construct a BVH using SAH 
	std::cout << "Building BVH using SAH" << std::endl;

	// 1. Initialize array of primitive info
	std::vector<primitiveInfo> primInfo(numPrimitives);
	for (size_t i = 0; i < numPrimitives; ++i) {
		aabb temp;
		objectList[i]->bounding_box(time0, time1, temp);
		primInfo[i] = { i, temp };
		//std::cout << i << " " << temp.min() << " " << temp.max() << std::endl;
	}

	// 2. Build BVH tree for primitives and process to produce a compactified bvh for traversal.

	std::cout << "Recursive build start" << std::endl;

	// A tree with N single-prim leaf nodes will have exactly N-1 internal nodes. 
	// As such, 2N nodes will always be enough to fully encapsulate the tree.  
	// Some nodes have more than 1 primitive, so we will have fewer nodes than this,
	// but we cannot know this prior to construction. Excess is therefore trimmed afterwards.
	nodes = AllocAligned<linearBVHNode>(2*numPrimitives); 

	int offset = 0;
	recursiveBuild(primitives, primInfo, 0, numPrimitives, time0, time1, &numNodes, &offset, orderedPrimitives);

	std::cout << "Recursive build complete" << std::endl;

	// Copy the ordered list of primitives back.
	for (int i = 0; i < numPrimitives; i++) {
		primitives[i] = orderedPrimitives[i];
	}

	// Cleanup
	orderedPrimitives.clear();
	primInfo.clear();

	// Trim excess memory
	nodes = ReallocAligned<linearBVHNode>(nodes, numNodes);

	std::cout << numNodes << " nodes created: " << numInteriorNodes << " interior, " << numLeafNodes << " leaves." << std::endl;
}

bool sahBVH::bounding_box(float t0, float t1, aabb& b) const {
	b = box;
	return true;
}


bool sahBVH::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
	// Traversal is done through the flattened representation of the bvh

	// Follow ray through nodes to find primitive intersections;

	bool hit = false;
	double closest_so_far = t_max;
	hit_record temp_record;

	// Store the number of intersection tests up to this point
	// This is incremented by 1 during primitive hit() functions and BVH traversal steps
	temp_record.numberOfIntersectionTests = rec.numberOfIntersectionTests;

	vec3 invDir(1 / r.direction().x(), 1 / r.direction().y(), 1 / r.direction().z());
	int dirIsNeg[3] = { invDir.x() < 0, invDir.y() < 0, invDir.z() < 0 };

	int toVisitOffset = 0, currentNodeIndex = 0;
	int nodesToVisit[64];

	while (true) {
		if (currentNodeIndex < 0) {
			return false;
		}

		// Obtain next node in the list to traverse
		const linearBVHNode* node = &nodes[currentNodeIndex];
		temp_record.numberOfIntersectionTests++; // New bvh traversal step so iterate 

		// Check ray against node
		if (node->bounds.hit(r, t_min, closest_so_far)) {
			// Hit bounding box
			if (node->nPrimitives > 0) {
				// Check for intersection with primitives
				for (int i = 0; i < node->nPrimitives; i++) {
					if (primitives[node->primitivesOffset + i]->hit(r, t_min, closest_so_far, temp_record)) {
						hit = true;
						if (temp_record.t < closest_so_far) {
							closest_so_far = temp_record.t;
							rec = temp_record;
						}
					}
				}

				// All nodes in the stack must be visited, even if we have a primitive hit. 
				// If the stack is empty, then we may break.

				if (toVisitOffset == 0) break;
				currentNodeIndex = nodesToVisit[--toVisitOffset];
			}
			else {
				// Interior node
				// Add further bvh node on the to do list, advance to the nearer node
				if (dirIsNeg[node->axis]) {
					nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
					currentNodeIndex = node->secondChildOffset;
				}
				else {
					nodesToVisit[toVisitOffset++] = node->secondChildOffset;
					currentNodeIndex += 1;
				}
			}
		}
		else {
			// Miss bounding box - find next node
			if (toVisitOffset == 0) break;
			currentNodeIndex = nodesToVisit[--toVisitOffset];
		}
	}
	return hit;

}
