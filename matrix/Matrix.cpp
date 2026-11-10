#include "Matrix.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <vector>
#include <random>
#include <string>

#include <omp.h>

//#include <sycl/sycl.hpp>

using namespace std;
namespace matrix
{
	Matrix::~Matrix()
	{
		if (matrix != nullptr)
		{
			delete[] matrix;
		}
	}

	Matrix::Matrix()
	{
		matrix = nullptr;
		dimensions[0] = 0;
		dimensions[1] = 0;
	}

	Matrix::Matrix(int x, int y)
	{
		if (x <= 0 || y <= 0)
		{
			string x = "Invalid matrix dimensions.\n";
			cerr << x;
			throw std::invalid_argument(x);
		}

		dimensions[0] = x;
		dimensions[1] = y;
		matrix = new float[x * y];
		std::fill(matrix, matrix + (x * y), 0.0);
	}

	Matrix::Matrix(const Matrix& other)
	{
		dimensions[0] = other.dimensions[0];
		dimensions[1] = other.dimensions[1];

		if (other.matrix != nullptr && dimensions[0] > 0 && dimensions[1] > 0)
		{
			matrix = new float[dimensions[0] * dimensions[1]];
			copy(other.matrix, other.matrix + (dimensions[0] * dimensions[1]), matrix);
		}
		else
		{
			matrix = nullptr;
		}
	}

	Matrix::Matrix(Matrix&& other) noexcept
	{
		dimensions[0] = other.dimensions[0];
		dimensions[1] = other.dimensions[1];
		matrix = other.matrix;

		other.dimensions[0] = 0;
		other.dimensions[1] = 0;
		other.matrix = nullptr;
	}

	void Matrix::PrintMatrix(const Matrix& a)
	{
		//Prints each value in the matrix
		for (int row = 0; row < a.dimensions[0]; row++)
		{
			for (int col = 0; col < a.dimensions[1]; col++)
			{
				cout << a[row][col] << " ";
			}
			cout << endl;
		}
	}

	void Matrix::PopulateRand(Matrix& a)
	{
		mt19937 rng(std::random_device{}());
		uniform_real_distribution<float> dist(-10000.0, 10000.0);

		//Populates the matrix's matrix with random integers from -1000 to 1000
		for (int row = 0; row < a.dimensions[0]; row++)
		{
			for (int col = 0; col < a.dimensions[1]; col++)
			{
				a[row][col] = dist(rng);
			}
		}
	}

	void Matrix::PopulateXavier(Matrix& a, int fanIn, int fanOut)
	{
		mt19937 rng(std::random_device{}());
		float limit = sqrt(6.0 / (a.dimensions[0] + a.dimensions[1]));
		uniform_real_distribution<float> dist(-limit, limit);

		for (int row = 0; row < a.dimensions[0]; row++)
		{
			for (int col = 0; col < a.dimensions[1]; col++)
			{
				a[row][col] = dist(rng);
			}
		}
	}

	Matrix Matrix::operator+(const Matrix& other) const
	{
		if (this->dimensions[0] == other.dimensions[0] && this->dimensions[1] == other.dimensions[1])
		{
			Matrix sum(this->dimensions[0], this->dimensions[1]);

			for (int row = 0; row < this->dimensions[0]; row++)
			{
				for (int col = 0; col < this->dimensions[1]; col++)
				{
					sum[row][col] = (*this)[row][col] + other[row][col];
				}
			}

			return sum;
		}
		else if (this->dimensions[0] == 1 && this->dimensions[1] == 1)
		{
			Matrix returnMe = (*this)[0][0] + other;

			return returnMe;
		}
		else if (other.dimensions[0] == 1 && other.dimensions[1] == 1)
		{
			Matrix returnMe = other[0][0] + *this;

			return returnMe;
		}
		else
		{
			string x = "Cannot add different sized matrices\n";
			cerr << x;
			throw std::invalid_argument(x);
		}
	}

	Matrix Matrix::operator+(const float val) const
	{
		Matrix sum = Matrix(this->dimensions[0], this->dimensions[1]);
		for (int row = 0; row < this->dimensions[0]; row++)
		{
			for (int col = 0; col < this->dimensions[1]; col++)
			{
				sum[row][col] = (*this)[row][col] + val;
			}
		}
		return sum;
	}

	Matrix operator+(const float val, const Matrix& matrix)
	{
		Matrix sum = Matrix(matrix.dimensions[0], matrix.dimensions[1]);
		for (int row = 0; row < matrix.dimensions[0]; row++)
		{
			for (int col = 0; col < matrix.dimensions[1]; col++)
			{
				sum[row][col] = matrix[row][col] + val;
			}
		}
		return sum;
	}

	Matrix& Matrix::operator+=(const Matrix& other)
	{
		if (this->dimensions[0] == other.dimensions[0] && this->dimensions[1] == other.dimensions[1])
		{
			for (int row = 0; row < this->dimensions[0]; row++)
			{
				for (int col = 0; col < this->dimensions[1]; col++)
				{
					(*this)[row][col] = (*this)[row][col] + other[row][col];
				}
			}

			return *this;
		}
		else if (this->dimensions[0] == 1 && this->dimensions[1] == 1)
		{
			*this = (*this)[0][0] + other;
			return *this;
		}
		else if (other.dimensions[0] == 1 && other.dimensions[1] == 1)
		{
			*this = other[0][0] + *this;
			return *this;
		}
		else
		{
			string x = "Cannot add different sized matrices\n";
			cerr << x;
			throw std::invalid_argument(x);
		}
	}

	Matrix& Matrix::operator+=(const float val)
	{
		for (int row = 0; row < this->dimensions[0]; row++)
		{
			for (int col = 0; col < this->dimensions[1]; col++)
			{
				(*this)[row][col] = (*this)[row][col] + val;
			}
		}
		return *this;
	}

	Matrix Matrix::operator-(const Matrix& other) const
	{
		if (this->dimensions[0] == other.dimensions[0] && this->dimensions[1] == other.dimensions[1])
		{
			Matrix diff(this->dimensions[0], this->dimensions[1]);

			for (int row = 0; row < this->dimensions[0]; row++)
			{
				for (int col = 0; col < this->dimensions[1]; col++)
				{
					diff[row][col] = (*this)[row][col] - other[row][col];
				}
			}

			return diff;
		}
		else if (this->dimensions[0] == 1 && this->dimensions[1] == 1)
		{
			Matrix returnMe = (*this)[0][0] - other;

			return returnMe;
		}
		else if (other.dimensions[0] == 1 && other.dimensions[1] == 1)
		{
			Matrix returnMe = *this - other[0][0];

			return returnMe;
		}
		else
		{
			string x = "Cannot add different sized matrices\n";
			cerr << x;
			throw std::invalid_argument(x);
		}
	}

	Matrix Matrix::operator-(const float val) const
	{
		Matrix diff = Matrix(this->dimensions[0], this->dimensions[1]);
		for (int row = 0; row < this->dimensions[0]; row++)
		{
			for (int col = 0; col < this->dimensions[1]; col++)
			{
				diff[row][col] = (*this)[row][col] - val;
			}
		}
		return diff;
	}

	Matrix operator-(const float val, const Matrix& other)
	{
		Matrix diff = Matrix(other.dimensions[0], other.dimensions[1]);
		for (int row = 0; row < other.dimensions[0]; row++)
		{
			for (int col = 0; col < other.dimensions[1]; col++)
			{
				diff[row][col] = val - other[row][col];
			}
		}
		return diff;
	}

	Matrix& Matrix::operator-=(const Matrix& other)
	{
		if (this->dimensions[0] == other.dimensions[0] && this->dimensions[1] == other.dimensions[1])
		{
			for (int row = 0; row < this->dimensions[0]; row++)
			{
				for (int col = 0; col < this->dimensions[1]; col++)
				{
					(*this)[row][col] = (*this)[row][col] - other[row][col];
				}
			}

			return *this;
		}
		else if (this->dimensions[0] == 1 && this->dimensions[1] == 1)
		{
			*this = *this - other[0][0];
			return *this;
		}
		else if (other.dimensions[0] == 1 && other.dimensions[1] == 1)
		{
			*this = other[0][0] - *this;
			return *this;
		}
		else
		{
			string x = "Cannot add different sized matrices\n";
			cerr << x;
			throw std::invalid_argument(x);
		}
	}

	Matrix& Matrix::operator-=(const float val)
	{
		for (int row = 0; row < this->dimensions[0]; row++)
		{
			for (int col = 0; col < this->dimensions[1]; col++)
			{
				(*this)[row][col] = (*this)[row][col] - val;
			}
		}
		return *this;
	}
	
	Matrix Matrix::operator*(const Matrix& other) const
	{
		//omp_set_num_threads(12);
		if (this->dimensions[1] == other.dimensions[0])
		{
			int n = this->dimensions[0];
			int k = this->dimensions[1];
			int m = other.dimensions[1];

			Matrix productMatrix(n, m);

#pragma omp parallel for 
			for (int row = 0; row < n; row++)
			{
				float* outRow = productMatrix[row];
				const float* aRow = (*this)[row];

				for (int ki = 0; ki < k; ki++)
				{
					const float a = aRow[ki];
					const float* bRow = other[ki];
					
					for (int col = 0; col < m; col++)
					{
						outRow[col] += a * bRow[col];
					}
				}
			}

			return productMatrix;
		}
		else
		{
			string x = "Cannot multiply different sized matrices.\n";
			cerr << x;
			throw std::invalid_argument(x);
		}
	}

	Matrix Matrix::operator*(const float other) const
	{
		Matrix productMatrix = Matrix(this->dimensions[0], this->dimensions[1]);
		for (int row = 0; row < this->dimensions[0]; row++)
		{
			for (int col = 0; col < this->dimensions[1]; col++)
			{
				productMatrix[row][col] = (*this)[row][col] * other;
			}
		}
		return productMatrix;
	}

	Matrix operator*(const float val, const Matrix& other)
	{
		Matrix productMatrix = Matrix(other.dimensions[0], other.dimensions[1]);
		for (int row = 0; row < other.dimensions[0]; row++)
		{
			for (int col = 0; col < other.dimensions[1]; col++)
			{
				productMatrix[row][col] = other[row][col] * val;
			}
		}
		return productMatrix;
	}

	Matrix& Matrix::operator*=(const Matrix& other)
	{
		//omp_set_num_threads(12);
		if (this->dimensions[1] == other.dimensions[0])
		{
			int n = this->dimensions[0];
			int k = this->dimensions[1];
			int m = other.dimensions[1];

			Matrix productMatrix(n, m);

#pragma omp parallel for 
			for (int row = 0; row < n; row++)
			{
				float* outRow = productMatrix[row];
				const float* aRow = (*this)[row];

				for (int ki = 0; ki < k; ki++)
				{
					const float a = aRow[ki];
					const float* bRow = other[ki];

					for (int col = 0; col < m; col++)
					{
						outRow[col] += a * bRow[col];
					}
				}
			}

			*this = std::move(productMatrix);
			return *this;
		}
		else
		{
			string x = "Cannot multiply different sized matrices.\n";
			cerr << x;
			throw std::invalid_argument(x);
		}
	}

	Matrix& Matrix::operator*=(const float other)
	{
		for (int row = 0; row < this->dimensions[0]; row++)
		{
			for (int col = 0; col < this->dimensions[1]; col++)
			{
				(*this)[row][col] = (*this)[row][col] * other;
			}
		}
		return *this;
	}

	Matrix Matrix::operator%(const Matrix& other)
	{
		if (this->dimensions[0] == other.dimensions[0] && this->dimensions[1] == other.dimensions[1])
		{
			Matrix productMatrix(this->dimensions[0], this->dimensions[1]);
			for (int row = 0; row < this->dimensions[0]; row++)
			{
				for (int col = 0; col < this->dimensions[1]; col++)
				{
					productMatrix[row][col] = (*this)[row][col] * other[row][col];
				}
			}
			return productMatrix;
		}
		else
		{
			string x = "Error.\n";
			cerr << x;
			throw std::invalid_argument(x);
		}
	}

	Matrix& Matrix::operator=(const float* other)
	{
		if (matrix != nullptr)
		{
			delete[] matrix;
		}
		matrix = new float[dimensions[0] * dimensions[1]];
		std::copy(other, other + dimensions[0] * dimensions[1], matrix);
		return *this;
	}

	Matrix& Matrix::operator=(const Matrix& other)
	{
		if (this != &other)
		{
			if (matrix != nullptr)
			{
				delete[] matrix;
			}
			dimensions[0] = other.dimensions[0];
			dimensions[1] = other.dimensions[1];
			if (other.matrix != nullptr && dimensions[0] > 0 && dimensions[1] > 0)
			{
				matrix = new float[dimensions[0] * dimensions[1]];
				std::copy(other.matrix, other.matrix + dimensions[0] * dimensions[1], matrix);
			}
			else
			{
				matrix = nullptr;
			}
		}
		return *this;
	}

	Matrix& Matrix::operator=(Matrix&& other) noexcept
	{
		if (this != &other)
		{
			if (matrix != nullptr)
			{
				delete[] matrix;
			}

			dimensions[0] = other.dimensions[0];
			dimensions[1] = other.dimensions[1];
			matrix = other.matrix;

			other.dimensions[0] = 0;
			other.dimensions[1] = 0;
			other.matrix = nullptr;
		}
		return *this;
	}

	const float* Matrix::operator[] (int index) const
	{
		assert(index >= 0 && index < dimensions[0]);
		return this->matrix + (index * this->dimensions[1]);
	}

	float* Matrix::operator[] (int index)
	{
		assert(index >= 0 && index < dimensions[0]);
		return this->matrix + (index * this->dimensions[1]);
	}

	float* Matrix::GetMatrix() const
	{
		return matrix;
	}

	int Matrix::Rows() const
	{
		return this->dimensions[0];
	}

	int Matrix::Cols() const
	{
		return this->dimensions[1];
	}

	std::pair<int, int> Matrix::Dimensions() const
	{
		return std::pair<int, int>{ this->dimensions[0], this->dimensions[1] };
	}

	template <typename T>
	std::vector<T> Matrix::ToArray(const Matrix v)
	{
		std::vector<T> flat;
		for (int row = 0; row < v.dimensions[0]; row++)
		{
			for (int col = 0; col < v.dimensions[1]; col++)
			{
				flat.push_back(v[row][col]);
			}
		}

		return flat;
	}

	template <typename T>
	std::vector<std::vector<T>> Matrix::Gridify(const std::vector<T> v, int n, int k)
	{
		std::vector<std::vector<T>> result(n, std::vector<T>(k));

		for (int row = 0; row < n; row++)
		{
			for (int col = 0; col < k; col++)
			{
				result[row][col] = v[row * k + col];
			}
		}

		return result;
	}

	Matrix Matrix::Transpose(const Matrix& matrix)
	{
		Matrix transposed = Matrix(matrix.dimensions[1], matrix.dimensions[0]);
		for (int row = 0; row < matrix.dimensions[0]; row++)
		{
			for (int col = 0; col < matrix.dimensions[1]; col++)
			{
				transposed[col][row] = matrix[row][col];
			}
		}
		return transposed;
	}
}
