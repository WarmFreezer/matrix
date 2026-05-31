#include "Matrix.h"
#include <random>
#include <chrono>

using namespace matrix;

int main()
{
	int n = 1000;
	int k = 1000;
	int m = 1000;

	Matrix a(n, k);
	Matrix b(k, m);

	Matrix::PopulateRand(a);
	Matrix::PopulateRand(b);

	auto start = std::chrono::high_resolution_clock::now();

	Matrix product = a * b;

	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;

	return 0;
}
