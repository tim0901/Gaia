//
//  matrix3.h
//  Gaia
//
//  Created by Alex Richardson on 16/09/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#pragma once

#ifndef MATRIX3_H
#define MATRIX3_H

#include "vec3.h"
#include "matrix2.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

class matrix3 {
public:

	matrix3() {}

	// Define using floats
	matrix3(float e00, float e01, float e02, float e10, float e11, float e12, float e20, float e21, float e22) { 
		// Row then column
		e[0] = e00; e[1] = e01; e[2] = e02;
		e[3] = e10; e[4] = e11; e[5] = e12;
		e[6] = e20; e[7] = e21; e[8] = e22;
	}
	// Define using row vectors
	matrix3(vec3 e0, vec3 e1, vec3 e2) {
		// Row then column
		e[0] = e0.x(); e[1] = e0.y(); e[2] = e0.z();
		e[3] = e1.x(); e[4] = e1.y(); e[5] = e1.z();
		e[6] = e2.x(); e[7] = e2.y(); e[8] = e2.z();
	}

	// Return matrix elements
	inline float e00() const { return e[0]; }
	inline float e01() const { return e[1]; }
	inline float e02() const { return e[2]; }
	inline float e10() const { return e[3]; }
	inline float e11() const { return e[4]; }
	inline float e12() const { return e[5]; }
	inline float e20() const { return e[6]; }
	inline float e21() const { return e[7]; }
	inline float e22() const { return e[8]; }

	inline float element(int i, int j) const {
		int element = (i * 3) + j;
		if (element > 8 || element < 0) {
			return 0;
		}
		else {
			return e[element];
		}
	}
	inline float element(vec2 ij) const {
		int element = (ij[0] * 3) + ij[1];
		if (element > 8 || element < 0) {
			return 0;
		}
		else {
			return e[element];
		}
	}

	// Return row / column vectors
	inline vec3 row(int i) const {
		if (i > 2 || i < 0) {
			return vec3(0, 0, 0);
		}
		else {
			return vec3(e[i * 3], e[i * 3 + 1], e[i * 3 + 2]);
		}
	}
	inline vec3 column(int j) const {
		if (j > 2 || j < 0) {
			return vec3(0, 0, 0);
		}
		else {
			return vec3(e[j], e[j + 3], e[j + 6]);
		}
	}

	// Defining operators
	inline const matrix3& operator + () const { return *this; }
	inline matrix3 operator - () const { return matrix3(-e[0], -e[1], -e[2], -e[3], -e[4], -e[5], -e[6], -e[7], -e[8]); }
	inline float operator [] (int i) const { return e[i]; }
	inline float& operator [] (int i) { return e[i]; }

	inline matrix3& operator += (const matrix3& m2);
	inline matrix3& operator -= (const matrix3& m2);

	inline matrix3& operator *= (const float t);
	inline matrix3& operator /= (const float t);

	// Determinant
	inline float det() const {
		return e[0] * minor(0, 0) - e[1] * minor(0, 1) + e[2] * minor(0, 2);
	}

	inline float minor(int row, int col) const {
		// The minor, M_i,j, is the determinant of the submatrix formed by
		// deleting the ith row and jth column.

		matrix2 minorMatrix;
		int matrixPointer = 0;

		// Create smaller matrix - ignoring the ith row and jth column
		for (int i = 0; i < 3; i++) {
			if (i != row) {
				for (int j = 0; j < 3; j++) {
					if (j != col) {
						minorMatrix[matrixPointer] = e[(i * 3) + j];
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
		matrix3 originalMatrix = *this;

		// Calculate determinant
		float determinant = originalMatrix.det();

		// If determinant = 0, return original matrix. Can't be inverted!
		if (determinant == 0) {
			return;
		}

		float invDet = 1 / determinant;

		// Find cofactors, divide by the determinant
		e[0] = originalMatrix.cofactor(0, 0) * invDet;
		e[1] = originalMatrix.cofactor(0, 1) * invDet;
		e[2] = originalMatrix.cofactor(0, 2) * invDet;
		e[3] = originalMatrix.cofactor(1, 0) * invDet;
		e[4] = originalMatrix.cofactor(1, 1) * invDet;
		e[5] = originalMatrix.cofactor(1, 2) * invDet;
		e[6] = originalMatrix.cofactor(2, 0) * invDet;
		e[7] = originalMatrix.cofactor(2, 1) * invDet;
		e[8] = originalMatrix.cofactor(2, 2) * invDet;

		// Finally, transpose the matrix
		transpose(); 
	}

	// Return the inverse of this matrix 
	inline matrix3 returnInverse() const {

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
		float D = cofactor(1, 0) * invDet;
		float E = cofactor(1, 1) * invDet;
		float F = cofactor(1, 2) * invDet;
		float G = cofactor(2, 0) * invDet;
		float H = cofactor(2, 1) * invDet;
		float I = cofactor(2, 2) * invDet;

		// Transpose and return
		return matrix3(A, D, G, B, E, H, C, F, I);
	}

	// Convert to the comatrix
	inline void comatrix() {
		matrix3 originalMatrix = *this;

		e[0] = originalMatrix.cofactor(0, 0);
		e[1] = originalMatrix.cofactor(0, 1);
		e[2] = originalMatrix.cofactor(0, 2);
		e[3] = originalMatrix.cofactor(1, 0);
		e[4] = originalMatrix.cofactor(1, 1);
		e[5] = originalMatrix.cofactor(1, 2);
		e[6] = originalMatrix.cofactor(2, 0);
		e[7] = originalMatrix.cofactor(2, 1);
		e[8] = originalMatrix.cofactor(2, 2);
	}

	// Return the comatrix
	inline matrix3 returnComatrix() const {
		return  matrix3(cofactor(0, 0), cofactor(0, 1), cofactor(0, 2), 
						cofactor(1, 0), cofactor(1, 1), cofactor(1, 2), 
						cofactor(2, 0), cofactor(2, 1), cofactor(2, 2));
	}

	// Convert to the adjugate of the matrix: adj(A)
	inline void adjugate() {
		// The adjugate matrix is the transpose of the comatrix
		comatrix();
		transpose();
	}

	// Return the adjugate of the matrix
	inline matrix3 returnAdjugte() const {
		// The adjugate matrix is the transpose of the comatrix
		return (returnComatrix()).returnTranspose();
	}

	// Transpose the matrix
	inline void transpose() {
		matrix3 originalMatrix = *this;

		e[0] = originalMatrix.e[0];
		e[1] = originalMatrix.e[3];
		e[2] = originalMatrix.e[6];
		e[3] = originalMatrix.e[1];
		e[4] = originalMatrix.e[4];
		e[5] = originalMatrix.e[7];
		e[6] = originalMatrix.e[2];
		e[7] = originalMatrix.e[5];
		e[8] = originalMatrix.e[8];
	}

	// Return the transpose of this matrix
	inline matrix3 returnTranspose() const {
		return  matrix3(e[0], e[3], e[6],
						e[1], e[4], e[7],
						e[2], e[5], e[8]);
	}

	float e[9];
};

inline matrix3& matrix3::operator += (const matrix3& m2) {
	e[0] += m2.e[0];
	e[1] += m2.e[1];
	e[2] += m2.e[2];
	e[3] += m2.e[3];
	e[4] += m2.e[4];
	e[5] += m2.e[5];
	e[6] += m2.e[6];
	e[7] += m2.e[7];
	e[8] += m2.e[8];
	return *this;
}


inline matrix3& matrix3::operator -= (const matrix3& m2) {
	e[0] -= m2.e[0];
	e[1] -= m2.e[1];
	e[2] -= m2.e[2];
	e[3] -= m2.e[3];
	e[4] -= m2.e[4];
	e[5] -= m2.e[5];
	e[6] -= m2.e[6];
	e[7] -= m2.e[7];
	e[8] -= m2.e[8];
	return *this;
}

inline matrix3& matrix3::operator *= (const float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	e[3] *= t;
	e[4] *= t;
	e[5] *= t;
	e[6] *= t;
	e[7] *= t;
	e[8] *= t;
	return *this;
}

inline matrix3& matrix3::operator /= (const float t) {
	if (t == 0) {
		return *this;
	}

	float k = 1.0 / t;
	
	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
	e[3] *= k;
	e[4] *= k;
	e[5] *= k;
	e[6] *= k;
	e[7] *= k;
	e[8] *= k;
	return *this;
}

// Addition
inline matrix3 operator + (const matrix3& m1, const matrix3& m2) {
	return  matrix3(m1.e[0] + m2.e[0], m1.e[1] + m2.e[1], m1.e[2] + m2.e[2],
					m1.e[3] + m2.e[3], m1.e[4] + m2.e[4], m1.e[5] + m2.e[5],
					m1.e[6] + m2.e[6], m1.e[7] + m2.e[7], m1.e[8] + m2.e[8]);
}

// Subtraction
inline matrix3 operator - (const matrix3& m1, const matrix3& m2) {
	return  matrix3(m1.e[0] - m2.e[0], m1.e[1] - m2.e[1], m1.e[2] - m2.e[2],
					m1.e[3] - m2.e[3], m1.e[4] - m2.e[4], m1.e[5] - m2.e[5],
					m1.e[6] - m2.e[6], m1.e[7] - m2.e[7], m1.e[8] - m2.e[8]);
}

// Matrix multiplication
inline matrix3 operator * (const matrix3& m1, const matrix3& m2) {
	return  matrix3(m1.e[0] * m2.e[0] + m1.e[1] * m2.e[3] + m1.e[2] * m2.e[6],
					m1.e[0] * m2.e[1] + m1.e[1] * m2.e[4] + m1.e[2] * m2.e[7],
					m1.e[0] * m2.e[2] + m1.e[1] * m2.e[5] + m1.e[2] * m2.e[8],
					m1.e[3] * m2.e[0] + m1.e[4] * m2.e[3] + m1.e[5] * m2.e[6],
					m1.e[3] * m2.e[1] + m1.e[4] * m2.e[4] + m1.e[5] * m2.e[7],
					m1.e[3] * m2.e[2] + m1.e[4] * m2.e[5] + m1.e[5] * m2.e[8],
					m1.e[6] * m2.e[0] + m1.e[7] * m2.e[3] + m1.e[8] * m2.e[6],
					m1.e[6] * m2.e[1] + m1.e[7] * m2.e[4] + m1.e[8] * m2.e[7],
					m1.e[6] * m2.e[2] + m1.e[7] * m2.e[5] + m1.e[8] * m2.e[8]);
}

// Scalar multiplication
inline matrix3 operator * (const matrix3& m1, const float t) {
	return  matrix3(m1.e[0] * t, m1.e[1] * t, m1.e[2] * t,
					m1.e[3] * t, m1.e[4] * t, m1.e[5] * t,
					m1.e[6] * t, m1.e[7] * t, m1.e[8] * t);
}

inline matrix3 operator * (const float t, const matrix3& m2) {
	return  matrix3(m2.e[0] * t, m2.e[1] * t, m2.e[2] * t,
					m2.e[3] * t, m2.e[4] * t, m2.e[5] * t,
					m2.e[6] * t, m2.e[7] * t, m2.e[8] * t);
}

// Scalar division
inline matrix3 operator / (const matrix3& m1, const float t) {
	if (t == 0) {
		return m1;
	}
	float k = 1 / t;
	return  matrix3(m1.e[0] * k, m1.e[1] * k, m1.e[2] * k,
					m1.e[3] * k, m1.e[4] * k, m1.e[5] * k,
					m1.e[6] * k, m1.e[7] * k, m1.e[8] * k);
}

// Multiplication with vec3
inline vec3 operator * (const matrix3& m1, const vec3& v2) {
	return vec3(m1.e[0] * v2.e[0] + m1.e[1] * v2.e[1] + m1.e[2] * v2.e[2],
				m1.e[3] * v2.e[0] + m1.e[4] * v2.e[1] + m1.e[5] * v2.e[2],
				m1.e[6] * v2.e[0] + m1.e[7] * v2.e[1] + m1.e[8] * v2.e[2]);
}

inline vec3 operator * (const vec3& v1, const matrix3& m2) {
	return vec3(m2.e[0] * v1.e[0] + m2.e[3] * v1.e[1] + m2.e[6] * v1.e[2],
				m2.e[1] * v1.e[0] + m2.e[4] * v1.e[1] + m2.e[7] * v1.e[2],
				m2.e[2] * v1.e[0] + m2.e[5] * v1.e[1] + m2.e[8] * v1.e[2]);
}

// For cout / cin
inline std::istream& operator >> (std::istream& is, matrix3& m) {
	is >> m.e[0] >> m.e[1] >> m.e[2] >> m.e[3] >> m.e[4] >> m.e[5] >> m.e[6] >> m.e[7] >> m.e[8];
	return is;
}

inline std::ostream& operator << (std::ostream& os, const matrix3& m) {
	os << m.e[0] << " " << m.e[1] << " " << m.e[2] << "\n" << m.e[3] << " " << m.e[4] << " " << m.e[5] << "\n" << m.e[6] << " " << m.e[7] << " " << m.e[8];
	return os;
}

#endif // !MATRIX3_H
