//
//  matrix4.h
//  Gaia
//
//  Created by Alex Richardson on 16/09/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#pragma once

#ifndef MATRIX4_H
#define MATRIX4_H

#include "vec4.h"
#include "matrix3.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

class matrix4 {
public:

	matrix4() {}

	// Define using floats
	matrix4(float e00, float e01, float e02, float e03, float e10, float e11, float e12, float e13, float e20, float e21, float e22, float e23, float e30, float e31, float e32, float e33) {
		// Row then collumn
		e[ 0] = e00; e[ 1] = e01; e[ 2] = e02; e[ 3] = e03;
		e[ 4] = e10; e[ 5] = e11; e[ 6] = e12; e[ 7] = e13;
		e[ 8] = e20; e[ 9] = e21; e[10] = e22; e[11] = e23;
		e[12] = e30; e[13] = e31; e[14] = e32; e[15] = e33;
	};
	// Define using row vectors
	matrix4(vec4 e0, vec4 e1, vec4 e2, vec4 e3) {
		// Row then column
		e[ 0] = e0.w(); e[ 1] = e0.x(); e[ 2] = e0.y(); e[ 3] = e0.z();
		e[ 4] = e1.w(); e[ 5] = e1.x(); e[ 6] = e1.y(); e[ 7] = e1.z();
		e[ 8] = e2.w(); e[ 9] = e2.x(); e[10] = e2.y(); e[11] = e2.z();
		e[12] = e3.w(); e[13] = e3.x(); e[14] = e3.y(); e[15] = e3.z();
	}

	// Return matrix elements
	inline float e00() const { return e[ 0]; }
	inline float e01() const { return e[ 1]; }
	inline float e02() const { return e[ 2]; }
	inline float e03() const { return e[ 3]; }
	inline float e10() const { return e[ 4]; }
	inline float e11() const { return e[ 5]; }
	inline float e12() const { return e[ 6]; }
	inline float e13() const { return e[ 7]; }
	inline float e20() const { return e[ 8]; }
	inline float e21() const { return e[ 9]; }
	inline float e22() const { return e[10]; }
	inline float e23() const { return e[11]; }
	inline float e30() const { return e[12]; }
	inline float e31() const { return e[13]; }
	inline float e32() const { return e[14]; }
	inline float e33() const { return e[15]; }

	inline float element(int i, int j) const {
		int element = (i * 4) + j;
		if (element > 15 || element < 0) {
			return 0;
		}
		else {
			return e[element];
		}
	}
	inline float element(vec4 ij) const {
		int element = (ij[0] * 4) + ij[1];
		if (element > 15 || element < 0) {
			return 0;
		}
		else {
			return e[element];
		}
	}

	// Return row / column vectors
	inline vec4 row(int i) const {
		if (i > 3 || i < 0) {
			return vec4(0, 0, 0, 0);
		}
		else {
			return vec4(e[i * 4], e[i * 4 + 1], e[i * 4 + 2], e[i * 4 + 3]);
		}
	}
	inline vec4 column(int j) const {
		if (j > 3 || j < 0) {
			return vec4(0, 0, 0, 0);
		}
		else {
			return vec4(e[j], e[j + 4], e[j + 8], e[j + 12]);
		}
	}

	// Defining operators
	inline const matrix4& operator + () const { return *this; }
	inline matrix4 operator - () const { return matrix4(-e[0], -e[1], -e[2], -e[3], -e[4], -e[5], -e[6], -e[7], -e[8], -e[9], -e[10], -e[11], -e[12], -e[13], -e[14], -e[15]); }
	inline float operator [] (int i) const { return e[i]; }
	inline float& operator [] (int i) { return e[i]; }

	inline matrix4& operator += (const matrix4& m2); 
	inline matrix4& operator -= (const matrix4& m2); 

	inline matrix4& operator *= (const float t); 
	inline matrix4& operator /= (const float t); 

	// Determinant - seems to be correct?
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

	inline float minor(int row, int col) const {
		// The minor, M_i,j, is the determinant of the submatrix formed by
		// deleting the ith row and jth column.

		matrix3 minorMatrix;
		int matrixPointer = 0;

		// Create smaller matrix - ignoring the ith row and jth column
		for (int i = 0; i < 4; i++) {
			if (i != row) {
				for (int j = 0; j < 4; j++) {
					if (j != col) {
						minorMatrix[matrixPointer] = e[(i * 4) + j];
						matrixPointer++;
					}
				}
			}
		}

		// Get the determinant of the submatrix
		return minorMatrix.det();
	}

	inline float cofactor(int i, int j) const {
		// C_i,j = M_i,j * (-1)^(i+j)

		return minor(i, j) * pow(-1, i + j);
	}

	// Invert the matrix. Assumes that the matrix is invertible. TODO: add more checks
	inline void invert() {
		matrix4 originalMatrix = *this;

		// Calculate determinant
		float determinant = originalMatrix.det();

		// If determinant = 0, return original matrix. Can't be inverted!
		if (determinant == 0) {
			return;
		}

		float invDet = 1 / determinant;

		// Find cofactors, divide by the determinant
		e[ 0] = originalMatrix.cofactor(0, 0) * invDet;
		e[ 1] = originalMatrix.cofactor(0, 1) * invDet;
		e[ 2] = originalMatrix.cofactor(0, 2) * invDet;
		e[ 3] = originalMatrix.cofactor(0, 3) * invDet;
		e[ 4] = originalMatrix.cofactor(1, 0) * invDet;
		e[ 5] = originalMatrix.cofactor(1, 1) * invDet;
		e[ 6] = originalMatrix.cofactor(1, 2) * invDet;
		e[ 7] = originalMatrix.cofactor(1, 3) * invDet;
		e[ 8] = originalMatrix.cofactor(2, 0) * invDet;
		e[ 9] = originalMatrix.cofactor(2, 1) * invDet;
		e[10] = originalMatrix.cofactor(2, 2) * invDet;
		e[11] = originalMatrix.cofactor(2, 3) * invDet;
		e[12] = originalMatrix.cofactor(3, 0) * invDet;
		e[13] = originalMatrix.cofactor(3, 1) * invDet;
		e[14] = originalMatrix.cofactor(3, 2) * invDet;
		e[15] = originalMatrix.cofactor(3, 3) * invDet;

		// Finally,  transpose the matrix
		transpose();
	}

	inline matrix4 returnInverse() const {
		
		// Calculate determinant
		float determinant = det();

		// If determinant = 0, return original matrix. Can't be inverted!
		if (determinant == 0) {
			return *this;
		}

		float invDet = 1 / determinant;

		// Find cofactors, divide by the determinant
		float A = cofactor(0, 0) * invDet;
		float B = cofactor(0, 1) * invDet;
		float C = cofactor(0, 2) * invDet;
		float D = cofactor(0, 3) * invDet;
		float E = cofactor(1, 0) * invDet;
		float F = cofactor(1, 1) * invDet;
		float G = cofactor(1, 2) * invDet;
		float H = cofactor(1, 3) * invDet;
		float I = cofactor(2, 0) * invDet;
		float J = cofactor(2, 1) * invDet;
		float K = cofactor(2, 2) * invDet;
		float L = cofactor(2, 3) * invDet;
		float M = cofactor(3, 0) * invDet;
		float N = cofactor(3, 1) * invDet;
		float O = cofactor(3, 2) * invDet;
		float P = cofactor(3, 3) * invDet;

		// Transpose and return
		return matrix4(A, E, I, M, B, F, J, N, C, G, K, O, D, H, L, P);
	}

	// Convert to the comatrix
	inline void comatrix() {
		matrix4 originalMatrix = *this;

		e[ 0] = originalMatrix.cofactor(0, 0);
		e[ 1] = originalMatrix.cofactor(0, 1);
		e[ 2] = originalMatrix.cofactor(0, 2);
		e[ 3] = originalMatrix.cofactor(0, 3);
		e[ 4] = originalMatrix.cofactor(1, 0);
		e[ 5] = originalMatrix.cofactor(1, 1);
		e[ 6] = originalMatrix.cofactor(1, 2);
		e[ 7] = originalMatrix.cofactor(1, 3);
		e[ 8] = originalMatrix.cofactor(2, 0);
		e[ 9] = originalMatrix.cofactor(2, 1);
		e[10] = originalMatrix.cofactor(2, 2);
		e[11] = originalMatrix.cofactor(2, 3);
		e[12] = originalMatrix.cofactor(3, 0);
		e[13] = originalMatrix.cofactor(3, 1);
		e[14] = originalMatrix.cofactor(3, 2);
		e[15] = originalMatrix.cofactor(3, 3);
	}

	// Return the comatrix
	inline matrix4 returnComatrix() const {
		return  matrix4(cofactor(0, 0), cofactor(0, 1), cofactor(0, 2), cofactor(0,3), 
						cofactor(1, 0), cofactor(1, 1), cofactor(1, 2), cofactor(1,3),
						cofactor(2, 0), cofactor(2, 1), cofactor(2, 2), cofactor(2,3),
						cofactor(3, 0), cofactor(3, 1), cofactor(3, 2), cofactor(3, 3));
	}

	// Convert to the adjugate of the matrix: adj(A)
	inline void adjugate() {
		// The adjugate matrix is the transpose of the comatrix
		comatrix();
		transpose();
	}

	// Return the adjugate of the matrix
	inline matrix4 returnAdjugte() const {
		// The adjugate matrix is the transpose of the comatrix
		return (returnComatrix()).returnTranspose();
	}

	// Transpose the matrix
	inline void transpose() {
		matrix4 originalMatrix = *this;
		e[ 0] = originalMatrix.e[ 0];
		e[ 1] = originalMatrix.e[ 4];
		e[ 2] = originalMatrix.e[ 8];
		e[ 3] = originalMatrix.e[12];
		e[ 4] = originalMatrix.e[ 1];
		e[ 5] = originalMatrix.e[ 5];
		e[ 6] = originalMatrix.e[ 9];
		e[ 7] = originalMatrix.e[13];
		e[ 8] = originalMatrix.e[ 2];
		e[ 9] = originalMatrix.e[ 6];
		e[10] = originalMatrix.e[10];
		e[11] = originalMatrix.e[14];
		e[12] = originalMatrix.e[ 3];
		e[13] = originalMatrix.e[ 7];
		e[14] = originalMatrix.e[11];
		e[15] = originalMatrix.e[15];
	}

	// Return the transpose of this matrix
	inline matrix4 returnTranspose() const {
		return  matrix4(e[0], e[4], e[ 8], e[12],
						e[1], e[5], e[ 9], e[13],
						e[2], e[6], e[10], e[14],
						e[3], e[7], e[11], e[15]);
	}

	float e[16];
};

inline matrix4& matrix4::operator += (const matrix4& m2) {
	e[ 0] += m2.e[ 0];
	e[ 1] += m2.e[ 1];
	e[ 2] += m2.e[ 2];
	e[ 3] += m2.e[ 3];
	e[ 4] += m2.e[ 4];
	e[ 5] += m2.e[ 5];
	e[ 6] += m2.e[ 6];
	e[ 7] += m2.e[ 7];
	e[ 8] += m2.e[ 8];
	e[ 9] += m2.e[ 9];
	e[10] += m2.e[10];
	e[11] += m2.e[11];
	e[12] += m2.e[12];
	e[13] += m2.e[13];
	e[14] += m2.e[14];
	e[15] += m2.e[15];
	return *this;
}

inline matrix4& matrix4::operator -= (const matrix4& m2) {
	e[ 0] -= m2.e[ 0];
	e[ 1] -= m2.e[ 1];
	e[ 2] -= m2.e[ 2];
	e[ 3] -= m2.e[ 3];
	e[ 4] -= m2.e[ 4];
	e[ 5] -= m2.e[ 5];
	e[ 6] -= m2.e[ 6];
	e[ 7] -= m2.e[ 7];
	e[ 8] -= m2.e[ 8];
	e[ 9] -= m2.e[ 9];
	e[10] -= m2.e[10];
	e[11] -= m2.e[11];
	e[12] -= m2.e[12];
	e[13] -= m2.e[13];
	e[14] -= m2.e[14];
	e[15] -= m2.e[15];
	return *this;
}

inline matrix4& matrix4::operator *= (const float t) {
	e[ 0] *= t;
	e[ 1] *= t;
	e[ 2] *= t;
	e[ 3] *= t;
	e[ 4] *= t;
	e[ 5] *= t;
	e[ 6] *= t;
	e[ 7] *= t;
	e[ 8] *= t;
	e[ 9] *= t;
	e[10] *= t;
	e[11] *= t;
	e[12] *= t;
	e[13] *= t;
	e[14] *= t;
	e[15] *= t;
	return *this;
}

inline matrix4& matrix4::operator /= (const float t) {
	if (t == 0) {
		return *this;
	}

	float k = 1.0 / t;

	e[ 0] *= k;
	e[ 1] *= k;
	e[ 2] *= k;
	e[ 3] *= k;
	e[ 4] *= k;
	e[ 5] *= k;
	e[ 6] *= k;
	e[ 7] *= k;
	e[ 8] *= k;
	e[ 9] *= k;
	e[10] *= k;
	e[11] *= k;
	e[12] *= k;
	e[13] *= k;
	e[14] *= k;
	e[15] *= k;
	return *this;
}

// Addition
inline matrix4 operator + (const matrix4& m1, const matrix4& m2) {
	return  matrix4(m1.e[ 0] + m2.e[ 0], m1.e[ 1] + m2.e[ 1], m1.e[ 2] + m2.e[ 2], m1.e[ 3] + m2.e[ 3],
					m1.e[ 4] + m2.e[ 4], m1.e[ 5] + m2.e[ 5], m1.e[ 6] + m2.e[ 6], m1.e[ 7] + m2.e[ 7],
					m1.e[ 8] + m2.e[ 8], m1.e[ 9] + m2.e[ 9], m1.e[10] + m2.e[10], m1.e[11] + m2.e[11],
					m1.e[12] + m2.e[12], m1.e[13] + m2.e[13], m1.e[14] + m2.e[14], m1.e[15] + m2.e[15]);
}

// Subtraction
inline matrix4 operator - (const matrix4& m1, const matrix4& m2) {
	return  matrix4(m1.e[ 0] - m2.e[ 0], m1.e[ 1] - m2.e[ 1], m1.e[ 2] - m2.e[ 2], m1.e[ 3] - m2.e[ 3],
					m1.e[ 4] - m2.e[ 4], m1.e[ 5] - m2.e[ 5], m1.e[ 6] - m2.e[ 6], m1.e[ 7] - m2.e[ 7],
					m1.e[ 8] - m2.e[ 8], m1.e[ 9] - m2.e[ 9], m1.e[10] - m2.e[10], m1.e[11] - m2.e[11],
					m1.e[12] - m2.e[12], m1.e[13] - m2.e[13], m1.e[14] - m2.e[14], m1.e[15] - m2.e[15]);
}

// Matrix multiplication
inline matrix4 operator * (const matrix4& m1, const matrix4& m2) {
	matrix4 temp = matrix4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	for (int i = 0; i < 4; i++) { // Each row
		for (int j = 0; j < 4; j++) { // Each column

			for (int k = 0; k < 4; k++) {
				temp[i * 4 + j] += m1.element(i, k) * m2.element(k, j);
			}
		}
	}
	return temp;
}

// Scalar multiplication
inline matrix4 operator * (const float t, const matrix4& m1) {
	return  matrix4(t * m1.e[ 0], t * m1.e[ 1], t * m1.e[ 2], t * m1.e[ 3],
					t * m1.e[ 4], t * m1.e[ 5], t * m1.e[ 6], t * m1.e[ 7],
					t * m1.e[ 8], t * m1.e[ 9], t * m1.e[10], t * m1.e[11],
					t * m1.e[12], t * m1.e[13], t * m1.e[14], t * m1.e[15]);
}

inline matrix4 operator * (const matrix4& m1, const float t) {
	return  matrix4(t * m1.e[ 0], t * m1.e[ 1], t * m1.e[ 2], t * m1.e[ 3],
					t * m1.e[ 4], t * m1.e[ 5], t * m1.e[ 6], t * m1.e[ 7],
					t * m1.e[ 8], t * m1.e[ 9], t * m1.e[10], t * m1.e[11],
					t * m1.e[12], t * m1.e[13], t * m1.e[14], t * m1.e[15]);
}

// Scalar Division
inline matrix4 operator / (const matrix4& m1, const float t) {
	if (t == 0) {
		return m1;
	}
	float k = 1 / t;
	return  matrix4(m1.e[ 0] * k, m1.e[ 1] * k, m1.e[ 2] * k, m1.e[ 3] * k,
					m1.e[ 4] * k, m1.e[ 5] * k, m1.e[ 6] * k, m1.e[ 7] * k,
					m1.e[ 8] * k, m1.e[ 9] * k, m1.e[10] * k, m1.e[11] * k,
					m1.e[12] * k, m1.e[13] * k, m1.e[14] * k, m1.e[15] * k);
}

// Multiplication with vec4
inline vec4 operator * (const matrix4& m1, const vec4& v2) {
	return vec4(m1.e[ 0] * v2.e[0] + m1.e[ 1] * v2.e[1] + m1.e[ 2] * v2.e[2] + m1.e[ 3] * v2.e[3],
				m1.e[ 4] * v2.e[0] + m1.e[ 5] * v2.e[1] + m1.e[ 6] * v2.e[2] + m1.e[ 7] * v2.e[3],
				m1.e[ 8] * v2.e[0] + m1.e[ 9] * v2.e[1] + m1.e[10] * v2.e[2] + m1.e[11] * v2.e[3],
				m1.e[12] * v2.e[0] + m1.e[13] * v2.e[1] + m1.e[14] * v2.e[2] + m1.e[15] * v2.e[3]);
}

inline vec4 operator * (const vec4& v1, const matrix4& m2) {
	return vec4(m2.e[0] * v1.e[0] + m2.e[4] * v1.e[1] + m2.e[ 8] * v1.e[2] + m2.e[12] * v1.e[3],
				m2.e[1] * v1.e[0] + m2.e[5] * v1.e[1] + m2.e[ 9] * v1.e[2] + m2.e[13] * v1.e[3],
				m2.e[2] * v1.e[0] + m2.e[6] * v1.e[1] + m2.e[10] * v1.e[2] + m2.e[14] * v1.e[3],
				m2.e[3] * v1.e[0] + m2.e[7] * v1.e[1] + m2.e[11] * v1.e[2] + m2.e[15] * v1.e[3]);
}

// For cout / cin
inline std::istream& operator >> (std::istream& is, matrix4& m) {
	is >> m.e[0] >> m.e[1] >> m.e[2] >> m.e[3] >> m.e[4] >> m.e[5] >> m.e[6] >> m.e[7] >> m.e[8] >> m.e[9] >> m.e[10] >> m.e[11] >> m.e[12] >> m.e[13] >> m.e[14] >> m.e[15];
	return is;
}

inline std::ostream& operator << (std::ostream& os, const matrix4& m) {
	os << m.e[0] << " " << m.e[1] << " " << m.e[2] << " " << m.e[3] << "\n" << m.e[4] << " " << m.e[5] << " " << m.e[6] << " " << m.e[7] << "\n" 
		<< m.e[8] << " " << m.e[9] << " " << m.e[10] << " " << m.e[11] << "\n" << m.e[12] << " " << m.e[13] << " " << m.e[14] << " " << m.e[15];
	return os;
}


#endif // !MATRIX4_H
