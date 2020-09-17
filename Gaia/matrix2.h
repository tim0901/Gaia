//
//  matrix2.h
//  Gaia
//
//  Created by Alex Richardson on 16/09/2020.
//  Copyright © 2020 Alex Richardson. All rights reserved.
//
#pragma once

#ifndef MATRIX2_H
#define MATRIX2_H

#include "vec2.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

class matrix2 {
public:

	matrix2() {} 

	// Define using floats
	matrix2(float e00, float e01, float e10, float e11) {
		// Row then column
		e[0] = e00; e[1] = e01;
		e[2] = e10; e[3] = e11;
	}
	// Define using row vectors
	matrix2(vec2 e0, vec2 e1) { 
		// Row then column
		e[0] = e0.x(); e[1] = e0.y();
		e[2] = e1.x(); e[3] = e1.y();
	}

	// Return matrix elements
	inline float e00() const { return e[0]; }
	inline float e01() const { return e[1]; }
	inline float e10() const { return e[2]; }
	inline float e11() const { return e[3]; }

	inline float element(int i, int j) const { 
		int element = (i * 2) + j;
		if (element > 3 || element < 0) {
			return 0;
		}
		else {
			return e[element];
		} 
	}
	inline float element(vec2 ij) const {
		int element = (ij[0] * 2) + ij[1];
		if (element > 3 || element < 0) {
			return 0;
		}
		else {
			return e[element];
		}
	}

	// Return row / column vectors
	inline vec2 row(int i) const {
		if (i > 1 || i < 0) {
			return vec2(0, 0);
		}
		else {
			return vec2(e[i * 2], e[i * 2 + 1]);
		}
	}
	inline vec2 column(int j) const {
		if (j > 1 || j < 0) {
			return vec2(0, 0);
		}
		else {
			return vec2(e[j], e[j + 2]);
		}
	}
	// Defining operators
	inline const matrix2& operator + () const { return *this; }
	inline matrix2 operator - () const { return matrix2(-e[0], -e[1], -e[2], -e[3]); }
	inline float operator [] (int i) const { return e[i]; }
	inline float& operator [] (int i) { return e[i]; }

	inline matrix2& operator += (const matrix2& m2); 
	inline matrix2& operator -= (const matrix2& m2); 

	inline matrix2& operator *= (const float t); 
	inline matrix2& operator /= (const float t); 

	// Determinant
	inline float det() const {
		return (e[0] * e[3]) - (e[1] * e[2]);
	}

	// Invert the matrix. Assumes that the matrix is invertible. TODO: add more checks
	inline void invert(){
		matrix2 originalMatrix = *this;

		// Calculate determinant
		float determinant = originalMatrix.det();

		// If determinant = 0, return original matrix. Can't be inverted!
		if (determinant == 0) {
			return;
		}

		float invDet = 1/determinant;

		// Move elements and divide by the determinant
		e[0] =  originalMatrix.e[3] * invDet;
		e[1] = -originalMatrix.e[1] * invDet;
		e[2] = -originalMatrix.e[2] * invDet;
		e[3] =  originalMatrix.e[0] * invDet;
	}

	// Return the inverse of this matrix 
	inline matrix2 returnInverse() const {

		// Calculate determinant
		float determinant = det();

		// If determinant = 0, return original matrix. Can't be inverted!
		if (determinant == 0) {
			return *this;
		}

		float invDet = 1 / determinant;

		return matrix2(e[3] * invDet, -e[1] * invDet, -e[2] * invDet, e[0] * invDet);
	}

	// Convert to the comatrix
	inline void comatrix() {
		matrix2 originalMatrix = *this;

		e[0] = originalMatrix.e[3];
		e[1] = -originalMatrix.e[2];
		e[2] = -originalMatrix.e[1];
		e[3] = originalMatrix.e[0];
	}

	// Return the comatrix
	inline matrix2 returnComatrix() {
		return matrix2(e[3], -e[2], -e[1], e[0]);
	}

	// Convert to the adjugate of the matrix: adj(A)
	inline void adjugate() {
		// The adjugate matrix is the transpose of the comatrix
		matrix2 originalMatrix = *this;

		e[0] =  originalMatrix.e[3];
		e[1] = -originalMatrix.e[1];
		e[2] = -originalMatrix.e[2];
		e[3] =  originalMatrix.e[0];
	}

	// Return the adjugate of the matrix
	inline matrix2 returnAdjugate() const {
		// The adjugate matrix is the transpose of the comatrix
		return matrix2(e[3], -e[1], -e[2], e[0]);
	}

	// Transpose the matrix
	inline void transpose() {
		matrix2 originalMatrix = *this;

		e[0] = originalMatrix.e[0];
		e[1] = originalMatrix.e[2];
		e[2] = originalMatrix.e[1];
		e[3] = originalMatrix.e[3];
	}

	// Return the transpose of this matrix
	inline matrix2 returnTranspose() const {
		return  matrix2(e[0], e[2], 
						e[1], e[3]);
	}

	float e[4];
};

inline matrix2& matrix2::operator += (const matrix2& m2) {
	e[0] += m2.e[0];
	e[1] += m2.e[1];
	e[2] += m2.e[2];
	e[3] += m2.e[3];
	return *this;
}

inline matrix2& matrix2::operator -= (const matrix2& m2) {
	e[0] -= m2.e[0];
	e[1] -= m2.e[1];
	e[2] -= m2.e[2];
	e[3] -= m2.e[3];
	return *this;
}

inline matrix2& matrix2::operator *= (const float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	e[3] *= t;
	return *this;
}

inline matrix2& matrix2::operator /= (const float t) {
	if (t == 0) {
		return *this;
	}

	float k = 1.0 / t;

	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
	e[3] *= k;
	return *this;
}

// Addition
inline matrix2 operator + (const matrix2& m1, const matrix2& m2) {
	return matrix2(m1.e[0] + m2.e[0], m1.e[1] + m2.e[1], m1.e[2] + m2.e[2], m1.e[3] + m2.e[3]);
}

// Subtraction
inline matrix2 operator - (const matrix2& m1, const matrix2& m2) {
	return matrix2(m1.e[0] - m2.e[0], m1.e[1] - m2.e[1], m1.e[2] - m2.e[2], m1.e[3] - m2.e[3]);
}

// Matrix multiplication
inline matrix2 operator * (const matrix2& m1, const matrix2& m2) {
	return matrix2(m1.e[0] * m2.e[0] + m1.e[1] * m2.e[2], m1.e[0] * m2.e[1] + m1.e[1] * m2.e[3],
				   m1.e[2] * m2.e[0] + m1.e[3] * m2.e[2], m1.e[2] * m2.e[1] + m1.e[3] * m2.e[3]);
}

// Scalar multiplication
inline matrix2 operator * (const matrix2& m1, const float t) {
	return matrix2(m1.e[0] * t, m1.e[1] * t, m1.e[2] * t, m1.e[3] * t);
}

inline matrix2 operator * (const float t, const matrix2& m2) {
	return matrix2(m2.e[0] * t, m2.e[1] * t, m2.e[2] * t, m2.e[3] * t);
}

// Scalar division
inline matrix2 operator / (const matrix2& m1, const float t) {
	if (t == 0) {
		return m1;
	}
	float k = 1 / t;
	return matrix2(m1.e[0] * k, m1.e[1] * k, m1.e[2] * k, m1.e[3] * k);
}

// Multiplication with vec2
inline vec2 operator * (const matrix2& m1, const vec2& v2) {
	return vec2(m1.e[0] * v2.e[0] + m1.e[1] * v2.e[1],
				m1.e[2] * v2.e[0] + m1.e[3] * v2.e[1]);
}

inline vec2 operator * (const vec2& v1, const matrix2& m2) {
	return vec2(v1.e[0] * m2.e[0] + v1.e[1] * m2.e[2],
				v1.e[0] * m2.e[1] + v1.e[1] * m2.e[3]);
}

// For cout / cin
inline std::istream& operator >> (std::istream& is, matrix2& m) {
	is >> m.e[0] >> m.e[1] >> m.e[2] >> m.e[3];
	return is;
}

inline std::ostream& operator << (std::ostream& os, const matrix2& m) {
	os << m.e[0] << " " << m.e[1] << "\n" << m.e[2] << " " << m.e[3];
	return os;
}


#endif // !MATRIX2_H
