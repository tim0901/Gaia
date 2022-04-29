#ifndef VECTOR_H
#define VECTOR_H

#include <array>
#include <algorithm>
#include <iostream>

template<typename T, size_t elements>
class Vector {
public:
	Vector() { }
	Vector(T t) { for (size_t i = 0; i < elements; i++) { data[i] = static_cast<T>(t); } }
	template<typename ...T1>
	Vector(T1 ...ts) : data{ static_cast<T>(ts)... } {}

	template<typename T1>
	Vector(const Vector<T1, elements>& v2) {
		for (int i = 0; i < elements; i++) {
			data[i] = static_cast<T>(v2[i]);
		}
	}

	inline T operator [] (int i) const { return data[i]; }
	inline T operator [] (size_t i) const { return data[i]; }
	inline T& operator [] (int i) { return data[i]; }
	inline T& operator [] (size_t i) { return data[i]; }
	
	inline const Vector& operator + () const { return *this; }
	inline Vector operator - () const {
		Vector<T, elements> ret = *this; std::transform(ret.begin(), ret.end(), ret.begin(), [](T t)->T {return -t; }); return ret;
	}

	inline bool operator == (const Vector<T, elements>& v2) {
		for (int i = 0; i < elements; i++) {
			if (data[i] != v2.data[i])
				return false;
		}
		return true;
	}

	inline bool operator != (const Vector<T, elements>& v2) {
		return !(*this == v2);
	}

	// Addition
	inline Vector<T, elements>& operator += (const Vector<T, elements>& v2);
	inline Vector<T, elements>& operator += (const double& t);

	// Subtraction
	inline Vector<T, elements>& operator -= (const Vector<T, elements>& v2);
	inline Vector<T, elements>& operator -= (const double& t);

	// Multiplication
	inline Vector<T, elements>& operator *= (const Vector<T, elements>& v2);
	inline Vector<T, elements>& operator *= (const double& t);

	// Division
	inline Vector<T, elements>& operator /= (const Vector<T, elements>& v2);
	inline Vector<T, elements>& operator /= (const double& t);

	template<typename T2>
	inline Vector& operator = (const Vector<T2, elements> v2) {
		Vector<T, elements> ret = *this; 
		for (int i = 0; i < elements; i++) {
			ret[i] = T(v2[i]);
		}
		return ret;
	}
	
	// Accessor functions for accessing uv/coordinate/colour elements - analogous to at()
	// Useful for code readability, but use with caution as all are available for vectors of any length
	// Eg. Calling a() on a 3-vector will throw an error
	inline T& u() { return data.at(0); }
	inline T& v() { return data.at(1); }
	inline T& x() { return data.at(0); }
	inline T& y() { return data.at(1); }
	inline T& z() { return data.at(2); }
	inline T& w() { return data.at(3); }
	inline T& r() { return data.at(0); }
	inline T& g() { return data.at(1); }
	inline T& b() { return data.at(2); }
	inline T& a() { return data.at(3); }
	inline T u() const { return data.at(0); }
	inline T v() const { return data.at(1); }
	inline T x() const { return data.at(0); }
	inline T y() const { return data.at(1); }
	inline T z() const { return data.at(2); }
	inline T w() const { return data.at(3); }
	inline T r() const { return data.at(0); }
	inline T g() const { return data.at(1); }
	inline T b() const { return data.at(2); }
	inline T a() const { return data.at(3); }

	inline void Clear(void* = 0) {
		for (int i = 0; i < elements; i++) {
			data[i] = T(0);
		}
	}

	void AddWithWeight(Vector const& src, float weight) {
		for (int i = 0; i < elements; i++) {
			data[i] += src.data[i] * weight;
		}
	}

	// C++ container named requirements
	inline constexpr std::array<T, elements>::iterator begin() noexcept { return data.begin(); }
	inline constexpr std::array<T, elements>::const_iterator begin() const noexcept { return data.begin(); }
	inline constexpr std::array<T, elements>::const_iterator cbegin() const noexcept { return data.cbegin(); }
	inline constexpr std::array<T, elements>::iterator end() noexcept { return data.end(); }
	inline constexpr std::array<T, elements>::const_iterator end() const noexcept { return data.end(); }
	inline constexpr std::array<T, elements>::const_iterator cend() const noexcept { return data.cend(); }

	inline constexpr std::array<T, elements>::size_type size() const noexcept { return data.size(); }
	inline constexpr std::array<T, elements>::size_type max_size() const noexcept { return data.max_size(); }
	inline constexpr bool empty() const noexcept { return data.empty(); }

	std::array<T, elements> data = {};
};

// Write vector to ostream
template<typename T, size_t elements>
inline std::ostream& operator << (std::ostream& os, const Vector<T, elements>& v) {
	for (size_t i = 0; i < elements; i++) {
		os << v[i];
		if (i < (elements - 1))
			os << " ";
	}
	return os;
}

// Vector addition
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator += (const Vector<T, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		data[i] += v2[i];
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator + (Vector<T, elements> v1, const Vector<T, elements>& v2) {
	v1 += v2;
	return v1;
}

// For allowing the use of mixed-precision vectors
template<size_t elements>
inline Vector<double, elements> operator + (Vector<double, elements> v1, const Vector<float, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] += (double)(v2[i]);
	}
	return v1;
}
template<size_t elements>
inline Vector<float, elements> operator + (Vector<float, elements> v1, const Vector<double, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] += (float)(v2[i]);
	}
	return v1;
}

// Scalar addition
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator += (const double& t) {
	for (size_t i = 0; i < elements; i++) {
		data[i] += (T)t;
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator + (Vector<T, elements> v1, const double& t) {
	v1 += (T)t;
	return v1;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator + (const double& t, Vector<T, elements> v1) {
	v1 += (T)t;
	return v1;
}

// Vector subtraction
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator -= (const Vector<T, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		data[i] -= v2[i];
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator - (Vector<T, elements> v1, const Vector<T, elements>& v2) {
	v1 -= v2;
	return v1;
}

// For allowing the use of mixed-precision vectors
template<size_t elements>
inline Vector<double, elements> operator - (Vector<double, elements> v1, const Vector<float, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] -= (double)(v2[i]);
	}
	return v1;
}
template<size_t elements>
inline Vector<float, elements> operator - (Vector<float, elements> v1, const Vector<double, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] -= (float)(v2[i]);
	}
	return v1;
}

// Scalar subtraction
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator -= (const double& t) {
	for (size_t i = 0; i < elements; i++) {
		data[i] -= (T)t;
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator - (Vector<T, elements> v1, const double& t) {
	v1 -= (T)t;
	return v1;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator - (const double& t, Vector<T, elements> v1) {
	v1 -= (T)t;
	return v1;
}

// Vector multiplication
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator *= (const Vector<T, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		data[i] *= v2[i];
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator * (Vector<T, elements> v1, const Vector<T, elements>& v2) {
	v1 *= v2;
	return v1;
}
// For allowing the use of mixed-precision vectors
template<size_t elements>
inline Vector<double, elements> operator * (Vector<double, elements> v1, const Vector<float, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] *= (double)(v2[i]);
	}
	return v1;
}
template<size_t elements>
inline Vector<float, elements> operator * (Vector<float, elements> v1, const Vector<double, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] *= (float)(v2[i]);
	}
	return v1;
}

// Scalar multiplication
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator *= (const double& t) {
	for (size_t i = 0; i < elements; i++) {
		data[i] *= (T)t;
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator * (Vector<T, elements> v1, const double& t) {
	v1 *= (T)t;
	return v1;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator * (const double& t, Vector<T, elements> v1) {
	v1 *= (T)t;
	return v1;
}

// Vector division
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator /= (const Vector<T, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		data[i] /= v2[i];
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator / (Vector<T, elements> v1, const Vector<T, elements>& v2) {
	v1 /= v2;
	return v1;
}

// For allowing the use of mixed-precision vectors
template<size_t elements>
inline Vector<double, elements> operator / (Vector<double, elements> v1, const Vector<float, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] /= (double)(v2[i]);
	}
	return v1;
}
template<size_t elements>
inline Vector<float, elements> operator / (Vector<float, elements> v1, const Vector<double, elements>& v2) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] /= (float)(v2[i]);
	}
	return v1;
}

// Scalar division
template<typename T, size_t elements>
inline Vector<T, elements>& Vector<T, elements>::operator /= (const double& t) {
	double inverse = 1.0 / (T)t;
	for (size_t i = 0; i < elements; i++) {
		data[i] *= inverse;
	}
	return *this;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator / (Vector<T, elements> v1, const double& t) {
	v1 /= (T)t;
	return v1;
}
template<typename T, size_t elements>
inline Vector<T, elements> operator / (const double& t, Vector<T, elements> v1) {
	for (size_t i = 0; i < elements; i++) {
		v1[i] = (T)t / v1[i];
	}
	return v1;
}

// Perform the dot product on two equal-length vectors
template<typename T, size_t elements>
double dot(const Vector<T, elements>& v1, const Vector<T, elements>& v2) {
	double ret = 0.0;
	for (int i = 0; i < elements; i++) {
		ret += v1[i] * v2[i];
	}
	return ret;
}

// Calculate the length of the vector
template<typename T, size_t elements>
double length(const Vector<T, elements>& v) {
	return sqrt(squared_length(v));
}

// Calculate the square of the length of the vector
template<typename T, size_t elements>
double squared_length(const Vector<T, elements>& v) {
	double ret = 0.0;
	for (auto i : v) {
		ret += (i * i);
	}
	return ret;
}

// Normalise the vector
template<typename T, size_t elements>
Vector<T, elements> unit_vector(const Vector<T, elements>& v) {
	return v / length(v);
}

// Calculate the cross product of two 3-vectors
// The cross product only exists in 3 and 7 dimensions
template<typename T>
Vector<T, 3> cross_product(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
	return Vector<T, 3>((v1[1] * v2[2] - v1[2] * v2[1]), (v1[2] * v2[0] - v1[0] * v2[2]), (v1[0] * v2[1] - v1[1] * v2[0]));
}

using std::abs;
template<typename T>
Vector<T, 3> abs(const Vector<T, 3>& v) {
	return Vector<T, 3>(abs(v[0]), abs(v[1]), abs(v[2]));
}

// Return the index containing the largest value
template<typename T, size_t elements>
int max_element(const Vector<T, elements>& v) {
	int maxI = 0;
	for (int i = 1; i < elements; i++) {
		if (v[maxI] < v[i])
			maxI = i;
	}
	return maxI;
}

// Return the index containing the smallest value
template<typename T, size_t elements>
int min_element(const Vector<T, elements>& v) {
	int minI = 0;
	for (int i = 0; i < elements; i++) {
		if (v[minI] > v[i])
			minI = i;
	}
	return minI;
}

using std::min;
// Given two equal-length vectors, return a vector containing the smallest value for each dimension, 
template<typename T, size_t elements>
Vector<T, elements> min(const Vector<T, elements>& v1, const Vector<T, elements>& v2) {
	Vector<T, elements> ret;
	for (size_t i = 0; i < elements; i++) {
		ret[i] = v1[i] < v2[i] ? v1[i] : v2[i];
	}
	return ret;
}
// Given two 2-vectors, return a 2-vector containing the smallest value for each dimension
template<typename T>
Vector<T, 2> min(const Vector<T, 2>& v1, const Vector<T, 2>& v2) {
	return Vector<T, 2>(v1[0] < v2[0] ? v1[0] : v2[0],
						v1[1] < v2[1] ? v1[1] : v2[1]);
}
// Given two 3-vectors, return a 3-vector containing the smallest value for each dimension
template<typename T>
Vector<T, 3> min(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
	return Vector<T, 3>(v1[0] < v2[0] ? v1[0] : v2[0],
						v1[1] < v2[1] ? v1[1] : v2[1],
						v1[2] < v2[2] ? v1[2] : v2[2]);
}
// Given two 4-vectors, return a 4-vector containing the smallest value for each dimension
template<typename T>
Vector<T, 4> min(const Vector<T, 4>& v1, const Vector<T, 4>& v2) {
	return Vector<T, 4>(v1[0] < v2[0] ? v1[0] : v2[0],
						v1[1] < v2[1] ? v1[1] : v2[1],
						v1[2] < v2[2] ? v1[2] : v2[2],
						v1[3] < v2[3] ? v1[3] : v2[3]);
}

using std::max;
// Given two equal-length vectors, return a vector containing the largest value for each dimension, 
template<typename T, size_t elements>
Vector<T, elements> max(const Vector<T, elements>& v1, const Vector<T, elements>& v2) {
	Vector<T, elements> ret;
	for (size_t i = 0; i < elements; i++) {
		ret[i] = v1[i] > v2[i] ? v1[i] : v2[i];
	}
	return ret;
}
// Given two 2-vectors, return a 3-vector containing the largest value for each dimension
template<typename T>
Vector<T, 2> max(const Vector<T, 2>& v1, const Vector<T, 2>& v2) {
	return Vector<T, 2>(v1[0] > v2[0] ? v1[0] : v2[0],
						v1[1] > v2[1] ? v1[1] : v2[1]);
}
// Given two 3-vectors, return a 3-vector containing the largest value for each dimension
template<typename T>
Vector<T, 3> max(const Vector<T, 3>& v1, const Vector<T, 3>& v2) {
	return Vector<T, 3>(v1[0] > v2[0] ? v1[0] : v2[0],
						v1[1] > v2[1] ? v1[1] : v2[1],
						v1[2] > v2[2] ? v1[2] : v2[2]);
}
// Given two 4-vectors, return a 4-vector containing the smallest value for each dimension
template<typename T>
Vector<T, 4> max(const Vector<T, 4>& v1, const Vector<T, 4>& v2) {
	return Vector<T, 4>(v1[0] > v2[0] ? v1[0] : v2[0],
						v1[1] > v2[1] ? v1[1] : v2[1],
						v1[2] > v2[2] ? v1[2] : v2[2],
						v1[3] > v2[3] ? v1[3] : v2[3]);
}

// Reorder vector indices as desired
template<typename T>
Vector<T, 2> permute(const Vector<T, 2>& v, int x, int y) {
	return Vector<T, 2>(v[x], v[y]);
}
template<typename T>
Vector<T, 3> permute(const Vector<T, 3>& v, int x, int y, int z) {
	return Vector<T, 3>(v[x], v[y], v[z]);
}
template<typename T>
Vector<T, 4> permute(const Vector<T, 4>& v, int x, int y, int z, int w) {
	return Vector<T, 4>(v[x], v[y], v[z], v[w]);
}
// TODO: permute vector indices for vectors of arbitrary length

// Shorthand definitions
using Vec2d = Vector<double, 2>;
using Vec3d = Vector<double, 3>;
using Vec4d = Vector<double, 4>;
using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;
using Vec4f = Vector<float, 4>;
using Vec2i = Vector<int, 2>;
using Vec3i = Vector<int, 3>;
using Vec4i = Vector<int, 4>;

#endif // !VECTOR_H
