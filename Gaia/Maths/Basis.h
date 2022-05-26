#ifndef BASIS_H
#define BASIS_H

import sml;
using namespace sml;

#include <algorithm>

class Basis
{
public:
	Basis() {}
	Basis(Vec3d a, Vec3d b, Vec3d c): u(a), v(b), w(c) {};

	void UpdateU(const Vec3d& vec) {
		u = vec;
		UpdateInverseTransformations();
	}
	void UpdateV(const Vec3d& vec) {
		v = vec;
		UpdateInverseTransformations();
	}
	void UpdateW(const Vec3d& vec) {
		w = vec;
		UpdateInverseTransformations();
	}

	Vec3d ToLocal(const Vec3d& vec) const {
		return vec.x() * invU + vec.y() * invV + vec.z() * invW;
	}

	Vec3d ToGlobal(const Vec3d& vec) const {
		return vec.x() * u + vec.y() * v + vec.z() * w;
	}

	Vec3d u, v, w;
	Vec3d invU, invV, invW;
	
	void UpdateInverseTransformations() {
		Mat33d temp({ u.x(), u.y(), u.z(), v.x(), v.y(), v.z(), w.x(), w.y(), w.z() });
		temp = transpose(temp);
		invU = temp.row_vector(0);
		invV = temp.row_vector(1);
		invW = temp.row_vector(2);
	}
};

Basis BasisFromVector(const Vec3d& normalised_vector);

#endif // !BASIS_H