#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <tuple>
#include "Vector.h"

// MATRICES ARE INDEXED ROW-BY-COLUMN
template<class T, size_t nrows, size_t ncols>
class Matrix {
public:
	Matrix() : data(std::vector<T>(nrows* ncols)) {}
	Matrix(const Matrix& m) : data(m.data) {}
	Matrix(std::vector<T> values) : data(values) {}
	Matrix(T i) : data(std::vector<T>(nrows* ncols, i)) {}
	Matrix(T arr[nrows][ncols]) : data(std::vector<T>(nrows* ncols)) {
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				data.at((i * ncols) + j) = arr[i][j];
			}
		}
	}
	Matrix(T arr[nrows * ncols]) : data(std::vector<T>(nrows* ncols)) {
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < ncols; j++) {
				data.at((i * ncols) + j) = arr[(i * ncols) + j];
			}
		}
	}

	// TODO: construct from sub-matrices (row/column vectors, squares eg pauli matrices)#

	// Operators

	// Access elements with M[row][column]
	inline std::vector<T>::iterator operator [] (int i) { return data.begin() + (i * ncols); }
	inline std::vector<T>::const_iterator operator [] (int i) const { return data.begin() + (i * ncols); }
	// (This class uses row-major memory ordering)

	inline const Matrix& operator + () const { return *this; }
	inline Matrix operator - () const { std::vector<T> ret = this->data; std::transform(ret.begin(), ret.end(), ret.begin(), [](T c) -> T {return -c; }); return Matrix(ret); }

	// Access elements with m.at(row, column) 
	inline T& at(int r, int c) { return data.at((r * ncols) + c); }
	inline T at(int r, int c) const { return data.at((r * ncols) + c); }

	// Fetch an individual row as a matrix of its own
	inline Matrix<T, 1, ncols> row(int r) const {
		Matrix<T, 1, ncols> ret;
		for (size_t i = 0; i < ncols; i++) {
			ret[0][i] = data[(r * ncols) + i];
		}
		return ret;
	}

	// Fetch an individual riw as a vector
	inline Vector<T, ncols> rowVector(int r) const {
		Vector<T, ncols> ret;
		for (size_t i = 0; i < ncols; i++) {
			ret[i] = data[(r * ncols) + i];
		}
		return ret;
	}

	// Fetch an individual column as a matrix of its own
	inline Matrix<T, nrows, 1> col(int c) const {
		Matrix<T, nrows, 1> ret;
		for (size_t i = 0; i < nrows; i++) {
			ret[i][0] = data[(i * ncols) + c];
		}
		return ret;
	}

	// Fetch an individual column as a vector
	inline Vector<T, nrows> colVector(int c) const {
		Vector<T, nrows> ret;
		for (size_t i = 0; i < nrows; i++) {
			ret[i] = data[(i * ncols) + c];
		}
		return ret;
	}



	// Addition
	inline Matrix<T, nrows, ncols>& operator += (const Matrix<T, nrows, ncols>& m2);
	template<size_t rows2, size_t cols2>
	inline Matrix<T, nrows, ncols>& operator += (const Matrix<T, rows2, cols2>& m2);
	inline Matrix<T, nrows, ncols>& operator += (const double t);

	// Subtraction
	inline Matrix<T, nrows, ncols>& operator -= (const Matrix<T, nrows, ncols>& m2);
	template<size_t rows2, size_t cols2>
	inline Matrix<T, nrows, ncols>& operator -= (const Matrix<T, rows2, cols2>& m2);
	inline Matrix<T, nrows, ncols>& operator -= (const double t);

	// Multiplication
	inline Matrix& operator *= (const double t);

	// Division
	inline Matrix& operator /= (const double t);

	inline bool operator == (const Matrix<T, nrows, ncols>& m2) { 
		if (data == m2.data)
			return true;
		else
			return false;
	}

	// C++ Container named requirements
	inline constexpr std::vector<T>::iterator begin() noexcept { return data.begin(); }
	inline constexpr std::vector<T>::const_iterator begin() const noexcept { return data.begin(); }
	inline constexpr std::vector<T>::iterator end() noexcept { return data.end(); }
	inline constexpr std::vector<T>::const_iterator end() const noexcept { return data.end(); }
	inline constexpr std::vector<T>::const_iterator cbegin() const noexcept { return data.cbegin(); }
	inline constexpr std::vector<T>::const_iterator cend() const noexcept { return data.cend(); }

	inline constexpr std::vector<T>::size_type size() const noexcept { return data.size(); }
	inline constexpr std::vector<T>::size_type max_size() const noexcept { return data.max_size(); }
	inline constexpr bool empty() const noexcept { return data.empty(); }

	inline constexpr size_t rows() const { return nrows; }
	inline constexpr size_t cols() const { return ncols; }
	std::vector<T> data;
};

// TODO: Minor submatrix
// TODO: Comatrix
// TODO: Adjugate (transpose of the comatrix)
// TODO: Cofactor
// TODO: Append rows/columns? eg 2x1 + 2x1 = 2x2

// Ostream << operator. Allows matrices to be printed to the console using eg cout
template<class T, size_t nrows, size_t ncols>
inline std::ostream& operator << (std::ostream& os, const Matrix<T, nrows, ncols>& m) {
	// For each column, find the longest element

	std::vector<int> column_widths(ncols);
	Matrix<std::string, nrows, ncols> stringMatrix;

	// Find the longest element in each column to determine their widths
	for (int i = 0; i < ncols; i++) {
		std::vector<std::string> stringVector(0);
		// Convert Ts to std::strings
		for (int j = 0; j < nrows; j++) {
			stringMatrix[j][i] = std::to_string(m[j][i]);
			// Trim trailing zeroes / decimal points from floating point types
			if (std::is_floating_point_v<T>) {
				stringMatrix[j][i].erase(stringMatrix[j][i].find_last_not_of('0') + 1, std::string::npos);
				stringMatrix[j][i].erase(stringMatrix[j][i].find_last_not_of('.') + 1, std::string::npos);
			}
			stringVector.push_back(stringMatrix[j][i]);
		}
		column_widths[i] = std::max_element(stringVector.begin(), stringVector.end(), [](std::string& const lhs, std::string& const rhs) {
			return lhs.size() < rhs.size();
			}
		)->size();
	}

	// Write matrix to os
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			// Pad extra spaces to ensure correct column spacing
			while (stringMatrix[i][j].size() < column_widths[j])
			{
				stringMatrix[i][j] += " ";
			}

			os << stringMatrix[i][j] << " ";
		}
		if(i < (nrows -1))
			os << "\n";
	}

	return os;
}

// Matrix addition - requires identical dimensions
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator + (const Matrix<T, nrows, ncols>& m1, const Matrix<T, nrows, ncols>& m2) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), m2.begin(), ret.begin(), [](T i, T j) -> T {return i + j; });	return ret;
}

// Matrix subtraction - requires identical dimensions
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator - (const Matrix<T, nrows, ncols>& m1, const Matrix<T, nrows, ncols>& m2) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), m2.begin(), ret.begin(), [](T i, T j) -> T {return i - j; });	return ret;
}

// Elementwise matrix division - requires identical dimensions
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator / (const Matrix<T, nrows, ncols>& m1, const Matrix<T, nrows, ncols>& m2) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), m2.begin(), ret.begin(), [](T i, T j) -> T {return i / j; });	return ret;
}

// Elementwise scalar addition
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator + (const Matrix<T, nrows, ncols>& m1, const T& t) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), ret.begin(), [t](T i) -> T {return i + t; });	return ret;
}
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator + (const T& t, const Matrix<T, nrows, ncols>& m1) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), ret.begin(), [t](T i) -> T {return i + t; });	return ret;
}

// Elementwise scalar subtraction
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator - (const Matrix<T, nrows, ncols>& m1, const T& t) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), ret.begin(), [t](T i) -> T {return i - t; });	return ret;
}
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator - (const T& t, const Matrix<T, nrows, ncols>& m1) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), ret.begin(), [t](T i) -> T {return i - t; });	return ret;
}

// Elementwise scalar multiplication
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator * (const Matrix<T, nrows, ncols>& m1, const T& t) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), ret.begin(), [t](T i) -> T {return i * t; });	return ret;
}
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator * (const T& t, const Matrix<T, nrows, ncols>& m1) {
	Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), ret.begin(), [t](T i) -> T {return i * t; });	return ret;
}

// Elementwise scalar division
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols> operator / (const Matrix<T, nrows, ncols>& m1, const T &t) {
	if (t == 0) throw std::logic_error("Divide by zero"); Matrix<T, nrows, ncols> ret = m1; std::transform(ret.begin(), ret.end(), ret.begin(), [t](T i) -> T {return i / t; });  return ret;
}

// Matrix += Matrix - identical dimensions
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator+= (const Matrix<T, nrows, ncols>& m2) {
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			data[(i * ncols) + j] += m2.at(i, j);
		}
	}
	return *this;
}

// Matrix += Matrix - different dimensions
template<class T, size_t nrows, size_t ncols>
template<size_t rows2, size_t cols2>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator += (const Matrix<T, rows2, cols2>& m2) {

	if (rows2 == 1 && ncols == cols2) {
		// m2 is a compatible row vector

		// Add row vector to each row. 
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < cols2; j++) {
				data[(i * cols2) + j] += m2.at(0, j);
			}
		}
	}
	else if (cols2 == 1 && nrows == rows2) {
		// m2 is a compatible column vector

		// Add column vector to each column. 
		for (int i = 0; i < rows2; i++) {
			for (int j = 0; j < ncols; j++) {
				data[(i * ncols) + j] += m2.at(i, 0);
			}
		}
	}
	else {
		throw std::domain_error("Incompatible matrix dimensions");
	}
	return *this;
}

// Matrix += scalar - add scalar to each element
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator+= (const double t) {
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			data[(i * ncols) + j] += t;
		}
	}
	return *this;
}

// Matrix -= Matrix - identical dimensions
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator-= (const Matrix<T, nrows, ncols>& m2) {
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			data[(i * ncols) + j] -= m2[i][j];
		}
	}
	return *this;
}

// Matrix -= Matrix - different dimensions
template<class T, size_t nrows, size_t ncols>
template<size_t rows2, size_t cols2>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator -= (const Matrix<T, rows2, cols2>& m2) {

	if (rows2 == 1 && ncols == cols2) {
		// m2 is a compatible row vector

		// Add row vector to each row. 
		for (int i = 0; i < nrows; i++) {
			for (int j = 0; j < cols2; j++) {
				data[(i * cols2) - j] += m2.at(0, j);
			}
		}
	}
	else if (cols2 == 1 && nrows == rows2) {
		// m2 is a compatible column vector

		// Add column vector to each column. 
		for (int i = 0; i < rows2; i++) {
			for (int j = 0; j < ncols; j++) {
				data[(i * ncols) - j] += m2.at(i, 0);
			}
		}
	}
	else {
		throw std::domain_error("Incompatible matrix dimensions");
	}
	return *this;
}

// Matrix -= scalar - subtract scalar from each element
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator-= (const double t) {
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			data[(i * ncols) + j] -= t;
		}
	}
	return *this;
}

// Matrix *= scalar
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator*= (const double t) {
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			data[(i * ncols) + j] *= t;
		}
	}
	return *this;
}

// Matrix Product
template<class T, size_t outside_rows, size_t inside_dim, size_t outside_cols>
inline Matrix<T, outside_rows, outside_cols> operator * (const Matrix<T, outside_rows, inside_dim>& m1, const Matrix<T, inside_dim, outside_cols>& m2) {
	Matrix<T, outside_rows, outside_cols> ret(0);
	for (int i = 0; i < outside_rows; i++) {
		for (int j = 0; j < outside_cols; j++) {
			for (int k = 0; k < inside_dim; k++) {
				ret[i][j] += (m1[i][k] * m2[k][j]);
			}
		}
	}
	return ret;
}

// Unrolled for small, common combinations
template<class T>
inline Matrix<T, 2, 2> operator * (const Matrix<T, 2, 2>& m1, const Matrix<T, 2, 2>& m2) {
	return Matrix<T, 2, 2>({ 
		m1.data[0] * m2.data[0] + m1.data[1] * m2.data[2], m1.data[0] * m2.data[1] + m1.data[1] * m2.data[3],
		m1.data[2] * m2.data[0] + m1.data[3] * m2.data[2], m1.data[2] * m2.data[1] + m1.data[3] * m2.data[3]});
}
template<class T>
inline Matrix<T, 3, 3> operator * (const Matrix<T, 3, 3>& m1, const Matrix<T, 3, 3>& m2) {
	return Matrix<T, 3, 3>({
		m1.data[0] * m2.data[0] + m1.data[1] * m2.data[3] + m1.data[2] * m2.data[6], m1.data[0] * m2.data[1] + m1.data[1] * m2.data[4] + m1.data[2] * m2.data[7], m1.data[0] * m2.data[2] + m1.data[1] * m2.data[5] + m1.data[2] * m2.data[8],
		m1.data[3] * m2.data[0] + m1.data[4] * m2.data[3] + m1.data[5] * m2.data[6], m1.data[3] * m2.data[1] + m1.data[4] * m2.data[4] + m1.data[5] * m2.data[7], m1.data[3] * m2.data[2] + m1.data[4] * m2.data[5] + m1.data[5] * m2.data[8],
		m1.data[6] * m2.data[0] + m1.data[7] * m2.data[3] + m1.data[8] * m2.data[6], m1.data[6] * m2.data[1] + m1.data[7] * m2.data[4] + m1.data[8] * m2.data[7], m1.data[6] * m2.data[2] + m1.data[7] * m2.data[5] + m1.data[8] * m2.data[8]});
}
template<class T>
inline Matrix<T, 4, 4> operator * (const Matrix<T, 4, 4>& m1, const Matrix<T, 4, 4>& m2) {
	return Matrix<T, 4, 4>({
		m1.data[0]  * m2.data[0] + m1.data[1]  * m2.data[4] + m1.data[2]  * m2.data[8] + m1.data[3]  * m2.data[12], m1.data[0]  * m2.data[1] + m1.data[1]  * m2.data[5] + m1.data[2]  * m2.data[9] + m1.data[3]  * m2.data[13], m1.data[0]  * m2.data[2] + m1.data[1]  * m2.data[6] + m1.data[2]  * m2.data[10] + m1.data[3]  * m2.data[14], m1.data[0]  * m2.data[3] + m1.data[1]  * m2.data[7] + m1.data[2]  * m2.data[11] + m1.data[3]  * m2.data[15],
		m1.data[4]  * m2.data[0] + m1.data[5]  * m2.data[4] + m1.data[6]  * m2.data[8] + m1.data[7]  * m2.data[12], m1.data[4]  * m2.data[1] + m1.data[5]  * m2.data[5] + m1.data[6]  * m2.data[9] + m1.data[7]  * m2.data[13], m1.data[4]  * m2.data[2] + m1.data[5]  * m2.data[6] + m1.data[6]  * m2.data[10] + m1.data[7]  * m2.data[14], m1.data[4]  * m2.data[3] + m1.data[5]  * m2.data[7] + m1.data[6]  * m2.data[11] + m1.data[7]  * m2.data[15],
		m1.data[8]  * m2.data[0] + m1.data[9]  * m2.data[4] + m1.data[10] * m2.data[8] + m1.data[11] * m2.data[12], m1.data[8]  * m2.data[1] + m1.data[9]  * m2.data[5] + m1.data[10] * m2.data[9] + m1.data[11] * m2.data[13], m1.data[8]  * m2.data[2] + m1.data[9]  * m2.data[6] + m1.data[10] * m2.data[10] + m1.data[11] * m2.data[14], m1.data[8]  * m2.data[3] + m1.data[9]  * m2.data[7] + m1.data[10] * m2.data[11] + m1.data[11] * m2.data[15],
		m1.data[12] * m2.data[0] + m1.data[13] * m2.data[4] + m1.data[14] * m2.data[8] + m1.data[15] * m2.data[12], m1.data[12] * m2.data[1] + m1.data[13] * m2.data[5] + m1.data[14] * m2.data[9] + m1.data[15] * m2.data[13], m1.data[12] * m2.data[2] + m1.data[13] * m2.data[6] + m1.data[14] * m2.data[10] + m1.data[15] * m2.data[14], m1.data[12] * m2.data[3] + m1.data[13] * m2.data[7] + m1.data[14] * m2.data[11] + m1.data[15] * m2.data[15]});
}
// Unrolled Matrix x Vector products
template<class T>
inline Matrix<T, 2, 1> operator * (const Matrix<T, 2, 2>& m1, const Matrix<T, 2, 1>& m2) {
	return Matrix<T, 2, 1>({ 
		m1.data[0] * m2.data[0] + m1.data[1] * m2.data[1], 
		m1.data[2] * m2.data[0] + m1.data[3] * m2.data[1] });
}
template<class T>
inline Vector<T, 3> operator * (const Matrix<T, 3, 3>& m, const Vector<T, 3>& v) {
	return Vector<T, 3>(m.data[0] * v.data[0] + m.data[1] * v.data[1] + m.data[2] * v.data[2],
		m.data[3] * v.data[0] + m.data[4] * v.data[1] + m.data[5] * v.data[2],
		m.data[6] * v.data[0] + m.data[7] * v.data[1] + m.data[8] * v.data[2]);
}
template<class T>
inline Vector<T, 3> operator * (const Vector<T, 3>& v, const Matrix<T, 3, 3>& m) {
	return Vector<T, 3>(v.data[0] * m.data[0] + v.data[1] * m.data[3] + v.data[2] * m.data[6],
		v.data[0] * m.data[1] + v.data[1] * m.data[4] + v.data[2] * m.data[7], 
		v.data[0] * m.data[2] + v.data[1] * m.data[5] + v.data[2] * m.data[8]);
}
template<class T>
inline Matrix<T, 1, 2> operator * (const Matrix<T, 1, 2>& m1, const Matrix<T, 2, 2>& m2) {
	return Matrix<T, 1, 2>({ 
		m1.data[0] * m2.data[0] + m1.data[1] * m2.data[2], m1.data[0] * m2.data[1] + m1.data[1] * m2.data[3]});
}
template<class T>
inline Matrix<T, 3, 1> operator * (const Matrix<T, 3, 3>& m1, const Matrix<T, 3, 1>& m2) {
	return Matrix<T, 3, 1>({
		m1.data[0] * m2.data[0] + m1.data[1] * m2.data[1] + m1.data[2] * m2.data[2],
		m1.data[3] * m2.data[0] + m1.data[4] * m2.data[1] + m1.data[5] * m2.data[2],
		m1.data[6] * m2.data[0] + m1.data[7] * m2.data[1] + m1.data[8] * m2.data[2]});
}
template<class T>
inline Matrix<T, 1, 3> operator * (const Matrix<T, 1, 3>& m1, const Matrix<T, 3, 3>& m2) {
	return Matrix<T, 1, 3>({
		m1.data[0] * m2.data[0] + m1.data[1] * m2.data[3] + m1.data[2] * m2.data[6], m1.data[0] * m2.data[1] + m1.data[1] * m2.data[4] + m1.data[2] * m2.data[7], m1.data[0] * m2.data[2] + m1.data[1] * m2.data[5] + m1.data[2] * m2.data[8]});
}
template<class T>
inline Matrix<T, 4, 1> operator * (const Matrix<T, 4, 4>& m1, const Matrix<T, 4, 1>& m2) {
	return Matrix<T, 4, 1>({
		m1.data[0]  * m2.data[0] + m1.data[1]  * m2.data[1] + m1.data[2]  * m2.data[2] + m1.data[3]  * m2.data[3],
		m1.data[4]  * m2.data[0] + m1.data[5]  * m2.data[1] + m1.data[6]  * m2.data[2] + m1.data[7]  * m2.data[3],
		m1.data[8]  * m2.data[0] + m1.data[9]  * m2.data[1] + m1.data[10] * m2.data[2] + m1.data[11] * m2.data[3],
		m1.data[12] * m2.data[0] + m1.data[13] * m2.data[1] + m1.data[14] * m2.data[2] + m1.data[15] * m2.data[3]});
}
template<class T>
inline Matrix<T, 1, 4> operator * (const Matrix<T, 1, 4>& m1, const Matrix<T, 4, 4>& m2) {
	return Matrix<T, 1, 4>({
		m1.data[0] * m2.data[0] + m1.data[1] * m2.data[4] + m1.data[2] * m2.data[8] + m1.data[3] * m2.data[12], m1.data[0] * m2.data[1] + m1.data[1] * m2.data[5] + m1.data[2] * m2.data[9]  + m1.data[3] * m2.data[13], m1.data[0] * m2.data[2] + m1.data[1] * m2.data[6] + m1.data[2] * m2.data[10] + m1.data[3] * m2.data[14], m1.data[0] * m2.data[3] + m1.data[1] * m2.data[7] + m1.data[2] * m2.data[11] + m1.data[3] * m2.data[15]});
}

// Matrix /= scalar
template<class T, size_t nrows, size_t ncols>
inline Matrix<T, nrows, ncols>& Matrix<T, nrows, ncols>::operator/= (const double t) {
	if (t == 0) throw std::logic_error("Divide by zero");
	for (int i = 0; i < nrows; i++) {
		for (int j = 0; j < ncols; j++) {
			data[(i * ncols) + j] /= t;
		}
	}
	return *this;
}

// Transpose matrices
template<class T, size_t rows, size_t cols>
Matrix<T, cols, rows> transpose(Matrix <T, rows, cols> original) {
	Matrix<T, cols, rows> temp;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			temp[j][i] = original[i][j];
		}
	}
	return temp;
}

// Dot product
template<class T, size_t dim1, size_t dim2>
T dot(Matrix<T, dim1, dim2>& m1, Matrix<T, dim2, dim1>& m2) {
	// Dot-product between a row vector and a column vector of equal length
	T output = 0;
	if (dim1 == 1) {
		for (int i = 0; i < dim2; i++) {
			output += (m1.at(0,i) * m2.at(i,0));
		}
	}
	else if (dim2 == 1) {
		for (int i = 0; i < dim1; i++) {
			output += (m1.at(i,0) * m2.at(0, i));
		}
	}
	else {
		throw std::domain_error("Incompatible vector dimensions");
	}
	return output;
}

template<class T, size_t dim1, size_t dim2>
T dot(Matrix<T, dim1, dim2>& m1, Matrix<T, dim1, dim2>& m2) {
	// Dot-product between two vectors of equal length and orientation
	T output = 0;
	if (dim1 == 1) {
		for (int i = 0; i < dim2; i++) {
			output += (m1.at(0,i) * m2.at(0, i));
		}
	}
	else if (dim2 == 1) {
		for (int i = 0; i < dim1; i++) {
			output += (m1.at(i, 0) * m2.at(i, 0));
		}
	}
	else {
		throw std::domain_error("Incompatible vector dimensions");
	}
	return output;
}

//Take the absolute value of each component
template<class T, size_t rows, size_t cols >
Matrix<T, rows, cols> abs(Matrix<T, rows, cols>& m) {
	Matrix<T, rows, cols> ret = m;
	for (int &i : ret) {
		i = std::abs(i);
	}
	return ret;
}

// Element-wise matrix multiplication (Hadamard product)
template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> multiply_elements(const Matrix<T, rows, cols>& m1, const Matrix<T, rows, cols>& m2) {
	Matrix<T, rows, cols> ret = m1;
	std::transform(ret.begin(), ret.end(), m2.begin(), ret.begin(), [](T i, T j) -> T {return i * j; });
	return ret;
}

// Multiply each row by the contents of a row vector
template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> multiply_elements(const Matrix<T, rows, cols>& m1, const Matrix<T, 1, cols>& m2) {
	Matrix<T, rows, cols> ret = m1;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			ret[i][j] *= m2[0][j];
		}
	}
	return ret;
}

// Multiply each column by the contents of a column vector
template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> multiply_elements(const Matrix<T, rows, cols>& m1, const Matrix<T, rows, 1>& m2) {
	Matrix<T, rows, cols> ret = m1;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			ret[i][j] *= m2[i][0];
		}
	}
	return ret;
}

// Element-wise matrix division
template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> divide_elements(const Matrix<T, rows, cols>& m1, const Matrix<T, rows, cols>& m2) {
	Matrix<T, rows, cols> ret = m1;
	std::transform(ret.begin(), ret.end(), m2.begin(), ret.begin(), [](T i, T j) -> T {return i / j; });
	return ret;
}

// Divide each row by the contents of a row vector
template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> divide_elements(const Matrix<T, rows, cols>& m1, const Matrix<T, 1, cols>& m2) {
	Matrix<T, rows, cols> ret = m1;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			ret[i][j] /= m2[0][j];
		}
	}
	return ret;
}

// Divide each column by the contents of a column vector
template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> divide_elements(const Matrix<T, rows, cols>& m1, const Matrix<T, rows, 1>& m2) {
	Matrix<T, rows, cols> ret = m1;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			ret[i][j] /= m2[i][0];
		}
	}
	return ret;
}

template<class T, size_t dim>
std::tuple<Matrix<double, dim, dim>, Matrix<int, dim, dim>, int> LUPDecomposition(const Matrix<T, dim, dim> &m) {
	
	Matrix<double, dim, dim> A;
	for (int i = 0; i < m.size(); i++) {
		A.data[i] = m.data[i];
	}

	int numberOfPivots = dim;
	Matrix<int, dim, dim> pivot_matrix(0);

	int maxI = 0, j = 0, k = 0;
	T maxA, absA;
	T ptr;

	for (int i = 0; i < dim; i++) {
		maxA = 0.0;
		maxI = i;

		for (k = i; k < dim; k++) {
			if ((absA = fabs(A[k][i])) > maxA) {
				maxA = absA;
				maxI = k;
			}
		}

		if (maxI != i) {
			// Pivoting P
			j = pivot_matrix.data[i];
			pivot_matrix.data[i] = pivot_matrix.data[maxI];
			pivot_matrix.data[maxI] = j;

			// Pivot rows of A
			for (int r = 0; r < dim; r++) {
				ptr = A[i][r];
				A[i][r] = A[maxI][r];
				A[maxI][r] = ptr;
			}
			
			// We store the number of pivots in the final element of the unit permutation vector
			numberOfPivots++;
		}

		for (j = i + 1; j < dim; j++) {
			A[j][i] /= A[i][i];
			for (k = i + 1; k < dim; k++) {
				A[j][k] -= A[j][i] * A[i][k];
			}
		}
	}

	return std::make_tuple(A, pivot_matrix, numberOfPivots);
}

template<class T, size_t dim>
T det(const Matrix<T, dim, dim>& m) {

	// Returns the determinant of matrix m from its LUP decomposition

	Matrix<double, dim, dim> A;
	Matrix<int, dim, dim> P;
	int numberOfPermutations;

	std::tie(A, P, numberOfPermutations) = LUPDecomposition(m);

	T det = A[0][0];
	for (int i = 1; i < dim; i++) {
		det *= A[i][i];
	}

	return (numberOfPermutations - dim) % 2 == 0 ? det : -det;
}

template<class T, size_t dim>
Matrix<T, dim, dim> inverse(const Matrix<T, dim, dim>& m) {

	// Returns the inverse of matrix m from its LUP decomposition

	Matrix<double, dim, dim> A;
	Matrix<int, dim, dim> P;
	int numberOfPermutations = 0;

	Matrix<T, dim, dim> ret(0);

	std::tie(A, P, numberOfPermutations) = LUPDecomposition(m);

	for (int j = 0; j < dim; j++) {
		for (int i = 0; i < dim; i++) {
			ret[i][j] = (P.data[i] == j ? 1.0 : 0.0);

			for (int k = 0; k < i; k++) {
				ret[i][j] -= A[i][k] * ret[k][j];
			}
		}
		for (int i = dim - 1; i >= 0; i--) {
			for (int k = i + 1; k < dim; k++) {
				ret[i][j] -= A[i][k] * ret[k][j];
			}
			ret[i][j] /= A[i][i];
		}
	}
	return ret;
}

template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> lerp(const Matrix<T, rows, cols>& m1, const Matrix<T, rows, cols>& m2, const T t) {
	return (m1 + ((m2 - m1) * t));
}

// Return the largest element
template<class T, size_t rows, size_t cols>
int max(const Matrix<T, rows, cols>& m) {
	return std::distance(m.begin(), std::max(m.begin(), m.end()));
}

// Return the index of the largest element
template<class T, size_t rows, size_t cols>
int max_element(const Matrix<T, rows, cols>& m) {
	return std::distance(m.begin(), std::max_element(m.begin(), m.end()));
}

// Return the smallest element
template<class T, size_t rows, size_t cols>
int min(const Matrix<T, rows, cols>& m) {
	return std::distance(m.begin(), std::min(m.begin(), m.end()));
}

// Return the index of the smallest element
template<class T, size_t rows, size_t cols>
int min_element(const Matrix<T, rows, cols>& m) {
	return std::distance(m.begin(), std::min_element(m.begin(), m.end()));
}

// Clamp
template<class T, size_t rows, size_t cols>
Matrix<T, rows, cols> clamp(const Matrix<T, rows, cols>& m, const T& t1, const T& t2) {
	Matrix<T, rows, cols> ret = m; std::transform(ret.begin(), ret.end(), ret.begin(), [t1, t2](T i) -> T {return std::clamp(i, t1, t2); });	return ret;
}

// Create a rotation matrix for a rotation about the X-axis
inline Matrix<float, 4, 4> RotateX(const float radians) {
	return Matrix<float, 4, 4>{ {   1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(radians), -sin(radians), 0.0f,
		0.0f, sin(radians), cos(radians), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f }};
}
// Create a rotation matrix for a rotation about the Y-axis
inline Matrix<float, 4, 4> RotateY(const float radians) {
	return Matrix<float, 4, 4>{ {   cos(radians), 0.0f, sin(radians), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(radians), 0.0f, cos(radians), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f }};
}
// Create a rotation matrix for a rotation about the Z-axis
inline Matrix<float, 4, 4> RotateZ(const float radians) {
	return Matrix<float, 4, 4>{ {   cos(radians), -sin(radians), 0.0f, 0.0f,
		sin(radians), cos(radians), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f }};
}

// Shorthand definitions
using Matrix22d = Matrix<double, 2, 2>;
using Matrix23d = Matrix<double, 2, 3>;
using Matrix24d = Matrix<double, 2, 4>;
using Matrix32d = Matrix<double, 3, 2>;
using Matrix33d = Matrix<double, 3, 3>;
using Matrix34d = Matrix<double, 3, 4>;
using Matrix42d = Matrix<double, 4, 2>;
using Matrix43d = Matrix<double, 4, 3>;
using Matrix44d = Matrix<double, 4, 4>;
using Matrix22f = Matrix<float, 2, 2>;
using Matrix23f = Matrix<float, 2, 3>;
using Matrix24f = Matrix<float, 2, 4>;
using Matrix32f = Matrix<float, 3, 2>;
using Matrix33f = Matrix<float, 3, 3>;
using Matrix34f = Matrix<float, 3, 4>;
using Matrix42f = Matrix<float, 4, 2>;
using Matrix43f = Matrix<float, 4, 3>;
using Matrix44f = Matrix<float, 4, 4>;
using Matrix22i = Matrix<int, 2, 2>;
using Matrix23i = Matrix<int, 2, 3>;
using Matrix24i = Matrix<int, 2, 4>;
using Matrix32i = Matrix<int, 3, 2>;
using Matrix33i = Matrix<int, 3, 3>;
using Matrix34i = Matrix<int, 3, 4>;
using Matrix42i = Matrix<int, 4, 2>;
using Matrix43i = Matrix<int, 4, 3>;
using Matrix44i = Matrix<int, 4, 4>;

#endif // !MATRIX_H
