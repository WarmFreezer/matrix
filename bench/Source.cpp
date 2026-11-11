#include <random>
#include <chrono>
#include <cmath>
#include <cuda_runtime.h>

#include "Cuda.h"
#include "Matrix.h"

using namespace matrix;
using namespace cuda;

static void gpu();
static void cpu();

int main()
{
	gpu();

	return 0;
}

static void gpu()
{
	cudaDeviceProp prop;
	cudaGetDeviceProperties(&prop, 0);
	std::cout << prop.name << " (sm_" << prop.major << prop.minor << ")\n";

	cudaFree(nullptr);

	for (int i = 256; i < 8192; i *= 2)
	{
		int n = i;
		int k = i;
		int m = i;

		Matrix a(n, k), b(k, m), out(n, m);

		Matrix::PopulateRand(a);
		Matrix::PopulateRand(b);


		// Way 1: CPU (your operator+)
		auto t0 = std::chrono::high_resolution_clock::now();
		
		Matrix cpu = a * b;
		
		auto t1 = std::chrono::high_resolution_clock::now();
		
		double cpuMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

		// Way 2: GPU (your cuda::Add)
		auto start = std::chrono::high_resolution_clock::now();

		cuda::Multiply(a.GetMatrix(), b.GetMatrix(), out.GetMatrix(), n, k, m);

		auto end = std::chrono::high_resolution_clock::now();

		double gpuTotalMs = std::chrono::duration<double, std::milli>(end - start).count();

		// Check the GPU answer against the CPU one over every element.
		// Relative to the largest CPU value: float sums of big numbers differ by large absolute amounts even when correct
		double maxDiff = 0, maxRef = 0;
		for (size_t e = 0; e < size_t(n) * m; e++)
		{
			maxDiff = std::fmax(maxDiff, std::fabs(double(out.GetMatrix()[e]) - cpu.GetMatrix()[e]));
			maxRef = std::fmax(maxRef, std::fabs(double(cpu.GetMatrix()[e])));
		}
		double relErr = maxRef > 0 ? maxDiff / maxRef : maxDiff;

		std::cout << i << "x" << i << "  CPU " << cpuMs << " ms  GPU+copies " << gpuTotalMs
			<< " ms. relative error " << relErr << (relErr < 1e-4 ? "  ok" : "  WRONG") << "\n";
	}
}

static void cpu()
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
}