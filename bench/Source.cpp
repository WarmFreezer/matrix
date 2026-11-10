#include "Matrix.h"
#include <random>
#include <chrono>

using namespace matrix;

int main()
{
	for (int i = 256; i < 8192; i *= 2)
	{
		int n = i;
		int k = i;
		int m = i;
		std::cout << "Matrix dimensions: " << n << "x" << k << " * " << k << "x" << m << std::endl;
	
		Matrix a(n, k);
		Matrix b(k, m);

		Matrix::PopulateRand(a);
		Matrix::PopulateRand(b);

		auto start = std::chrono::high_resolution_clock::now();

		Matrix product = a * b;

		auto end = std::chrono::high_resolution_clock::now();

		std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
	}

	return 0;
}
