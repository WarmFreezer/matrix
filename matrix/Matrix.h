#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <omp.h>

namespace matrix
{
	class Matrix
	{
	public:
		~Matrix();
		Matrix();
		Matrix(int x, int y);
		Matrix(const Matrix& other);
		Matrix(Matrix&& other) noexcept;

		static void PrintMatrix(const Matrix& a);
		static void PopulateRand(Matrix& a);
		static void PopulateXavier(Matrix& a, int fanIn, int fanOut);

		Matrix operator+(const Matrix& other) const;
		Matrix operator+(const float val) const;
		friend Matrix operator+(const float val, const Matrix& matrix);
		
		Matrix& operator+=(const Matrix& other);
		Matrix& operator+=(const float val);
		
		Matrix operator-(const Matrix& other) const;
		Matrix operator-(const float val) const;
		friend Matrix operator-(const float val, const Matrix& matrix);
		
		Matrix& operator-=(const Matrix& other);
		Matrix& operator-=(const float val);

		Matrix operator*(const Matrix& other) const;
		Matrix operator*(const float other) const;
		friend Matrix operator*(const float val, const Matrix& other);

		Matrix& operator*=(const Matrix& other);
		Matrix& operator*=(const float other);
		
		Matrix operator%(const Matrix& other);
		Matrix& operator=(const float* other);
		Matrix& operator=(const Matrix& other);
		Matrix& operator=(Matrix&& other) noexcept;
		const float* operator[] (int index) const;
		float* operator[] (int index);

		int Rows() const;
		int Cols() const;
		std::pair<int, int> Dimensions() const;
		float* GetMatrix() const;

		template <typename T>
		static std::vector<T> ToArray(const Matrix v);

		template <typename T>
		static std::vector<std::vector<T>> Gridify(const std::vector<T> v, int n, int k);

		static Matrix Transpose(const Matrix& matrix);

	private:
		int dimensions[2];
		float* matrix;
	};
}