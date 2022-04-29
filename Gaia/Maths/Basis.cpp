#include "Basis.h"

// Create a basis given a normalised vector
Basis BasisFromVector(const Vec3d& normalised_vector) {
	Basis ret;

	ret.u = normalised_vector;

	// Find whichever of u's dimensions is smallest and zero it to create v2
	if (std::abs(ret.u.x()) > std::abs(ret.u.y())) {
		ret.v = Vec3d(-ret.u.z(), 0.0, ret.u.x()) / sqrt(ret.u.x() * ret.u.x() + ret.u.z() * ret.u.z());
	}
	else {
		ret.v = Vec3d(0.0, ret.u.z(), -ret.u.y()) / sqrt(ret.u.y() * ret.u.y() + ret.u.z() * ret.u.z());
	}
	// Use the cross product to find the third basis vector
	ret.w = cross_product(ret.u, ret.v);

	ret.UpdateInverseTransformations();

	return ret;
}