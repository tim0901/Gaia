#pragma once
#ifndef MATRIX4_H
#define MATRIX4_H

#include "vec4.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

class matrix4 {
public:

	matrix4() {}
	matrix4(float e00, float e01, float e02, float e03, float e10, float e11, float e12, float e13, float e20, float e21, float e22, float e23, float e30, float e31, float e32, float e33) {
		//Row then collumn
		e[0] = e00; e[1] = e01; e[2] = e02; e[3] = e03;
		e[4] = e10; e[5] = e11; e[6] = e12; e[7] = e13;
		e[8] = e20; e[9] = e21; e[10] = e22; e[11] = e23;
		e[12] = e30; e[13] = e31; e[14] = e32; e[15] = e33;
	};

	//Return matrix elements
	inline float e00() const { return e[0]; }
	inline float e01() const { return e[1]; }
	inline float e02() const { return e[2]; }
	inline float e03() const { return e[3]; }
	inline float e10() const { return e[4]; }
	inline float e11() const { return e[5]; }
	inline float e12() const { return e[6]; }
	inline float e13() const { return e[7]; }
	inline float e20() const { return e[8]; }
	inline float e21() const { return e[9]; }
	inline float e22() const { return e[10]; }
	inline float e23() const { return e[11]; }
	inline float e30() const { return e[12]; }
	inline float e31() const { return e[13]; }
	inline float e32() const { return e[14]; }
	inline float e33() const { return e[15]; }

	//Defining operators
	inline const matrix4& operator + () const { return *this; }
	inline matrix4 operator - () const { return matrix4(-e[0], -e[1], -e[2], -e[3], -e[4], -e[5], -e[6], -e[7], -e[8], -e[9], -e[10], -e[11], -e[12], -e[13], -e[14], -e[15]); }
	inline float operator [] (int i) const { return e[i]; }
	inline float& operator [] (int i) { return e[i]; }

	inline matrix4& operator += (const matrix4& m2); //TODO
	inline matrix4& operator -= (const matrix4& m2); //TODO
	inline matrix4& operator *= (const matrix4& m2); //TODO
	inline matrix4& operator *= (const vec4& v2); //TODO
	//inline matrix4& operator /= (const matrix4& m2); -Matrix division doesn't exist. 
	inline matrix4& operator *= (const float t);
	inline matrix4& operator /= (const float t);

	//Determinant - seems to be correct?
	inline float det() const {
		return e[0] * (e[5] * (e[10] * e[15] - e[11] * e[14])
			- e[6] * (e[9] * e[15] - e[11] * e[13])
			+ e[7] * (e[9] * e[14] - e[10] * e[13])
			)
			- e[1] * (e[4] * (e[10] * e[15] - e[11] * e[14])
				- e[6] * (e[8] * e[15] - e[11] * e[12])
				+ e[7] * (e[8] * e[14] - e[10] * e[12])
				)
			+ e[2] * (e[4] * (e[9] * e[15] - e[11] * e[13])
				- e[5] * (e[8] * e[15] - e[11] * e[12])
				+ e[7] * (e[8] * e[13] - e[9] * e[12])
				)
			- e[3] * (e[4] * (e[9] * e[14] - e[10] * e[13])
				- e[5] * (e[8] * e[14] - e[10] * e[12])
				+ e[6] * (e[8] * e[13] - e[9] * e[12])
				);
	}

	//Return the inverse of the matrix. This assumes the matrix is invertible - need to add checks.
	inline matrix4 invert() const {
		matrix4 originalMatrix = *this;

		//If determinant = 0, return original matrix. Can't be inverted!
		if (originalMatrix.det() == 0) {
			return *this;
		}

		for (int i = 0; i < 4; i++) {

		}

		1 / originalMatrix.det();
	}

	inline float cofactor() const {

	}

	inline matrix4 adjoint() const {
		//Transpose of the cofactor matrix.
	}

	//Return the transpose of the matrix
	inline matrix4 transpose() const {
		//Transpose of matrix
		return matrix4(e[0], e[4], e[8], e[12],
			e[1], e[5], e[9], e[13],
			e[2], e[6], e[10], e[14],
			e[3], e[7], e[11], e[15]);
	}

	float e[16];
};

//Multiplication by scalar
inline matrix4 operator * (float t, matrix4 m1) {
	return matrix4(t * m1.e[0], t * m1.e[1], t * m1.e[2], t * m1.e[3],
		t * m1.e[4], t * m1.e[5], t * m1.e[6], t * m1.e[7],
		t * m1.e[8], t * m1.e[9], t * m1.e[10], t * m1.e[11],
		t * m1.e[12], t * m1.e[13], t * m1.e[14], t * m1.e[15]);
}

//Division by scalar
inline matrix4 operator / (matrix4 m1, float t) {
	return matrix4(m1.e[0] / t, m1.e[1] / t, m1.e[2] / t, m1.e[3] / t,
		m1.e[4] / t, m1.e[5] / t, m1.e[6] / t, m1.e[7] / t,
		m1.e[8] / t, m1.e[9] / t, m1.e[10] / t, m1.e[11] / t,
		m1.e[12] / t, m1.e[13] / t, m1.e[14] / t, m1.e[15] / t);
}

//Matrix addition
inline matrix4 operator + (matrix4 m1, matrix4 m2) {
	return matrix4(m1.e[0] + m2.e[0], m1.e[1] + m2.e[1], m1.e[2] + m2.e[2], m1.e[3] + m2.e[3],
		m1.e[4] + m2.e[4], m1.e[5] + m2.e[5], m1.e[6] + m2.e[6], m1.e[7] + m2.e[7],
		m1.e[8] + m2.e[8], m1.e[9] + m2.e[9], m1.e[10] + m2.e[10], m1.e[11] + m2.e[11],
		m1.e[12] + m2.e[12], m1.e[13] + m2.e[13], m1.e[14] + m2.e[14], m1.e[15] + m2.e[15]
	);
}

//Matrix subtraction
inline matrix4 operator - (matrix4 m1, matrix4 m2) {
	return matrix4(m1.e[0] - m2.e[0], m1.e[1] - m2.e[1], m1.e[2] - m2.e[2], m1.e[3] - m2.e[3],
		m1.e[4] - m2.e[4], m1.e[5] - m2.e[5], m1.e[6] - m2.e[6], m1.e[7] - m2.e[7],
		m1.e[8] - m2.e[8], m1.e[9] - m2.e[9], m1.e[10] - m2.e[10], m1.e[11] - m2.e[11],
		m1.e[12] - m2.e[12], m1.e[13] - m2.e[13], m1.e[14] - m2.e[14], m1.e[15] - m2.e[15]
	);
}


#endif // !MATRIX4_H
