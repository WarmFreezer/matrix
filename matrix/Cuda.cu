#include "Cuda.h"
#include <cuda_runtime.h>
#include <iostream>
#include <source_location>
#include <device_launch_parameters.h>

inline void CudaCheck(cudaError_t err, std::source_location loc = std::source_location::current())
{
	if (err != cudaSuccess)
	{
		std::cerr << "CUDA error in " << loc.file_name() << ":" << loc.line() << " - " << cudaGetErrorString(err) << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

namespace cuda
{
	__global__ void AddKernel(const float* a, const float* b, float* out, int n)
	{
		int i = blockIdx.x * blockDim.x + threadIdx.x;
		if (i < n)
		{
			out[i] = a[i] + b[i];
		}
	}

	void AddDevice(const float* dA, const float* dB, float* dOut, int n)
	{
		const int threads = 256;
		const int blocks = (n + threads - 1) / threads;  // round up
		AddKernel <<<blocks, threads>>> (dA, dB, dOut, n);
		CudaCheck(cudaGetLastError());                   // catches an invalid launch
	}

	void Add(const float* a, const float* b, float* out, int n)
	{
		size_t bytes = size_t(n) * sizeof(float);

		float* dA, * dB, * dOut;

		CudaCheck(cudaMalloc(&dA, bytes));
		CudaCheck(cudaMalloc(&dB, bytes));
		CudaCheck(cudaMalloc(&dOut, bytes));

		CudaCheck(cudaMemcpy(dA, a, bytes, cudaMemcpyHostToDevice));
		CudaCheck(cudaMemcpy(dB, b, bytes, cudaMemcpyHostToDevice));

		AddDevice(dA, dB, dOut, n);
		CudaCheck(cudaMemcpy(out, dOut, bytes, cudaMemcpyDeviceToHost));

		CudaCheck(cudaFree(dA));
		CudaCheck(cudaFree(dB));
		CudaCheck(cudaFree(dOut));
	}

	__global__ void MultiplyKernel(const float* a, const float* b, float* out, int n, int k, int m)
	{
		int row = blockIdx.y * blockDim.y + threadIdx.y;
		int col = blockIdx.x * blockDim.x + threadIdx.x;
		
		if (row < n && col < m)
		{
			float sum = 0.0f;
			for (int ki = 0; ki < k; ki++)
			{
				sum += a[row * k + ki] * b[ki * m + col];
			}
			out[row * m + col] = sum;
		}
	}

	void MultiplyDevice(const float* dA, const float* dB, float* dOut, int n, int k, int m)
	{
		const dim3 threads(16, 16);
		const dim3 blocks((m + 15) / 16, (n + 15) / 16);  // round up
		MultiplyKernel <<<blocks, threads>>> (dA, dB, dOut, n, k, m);
		CudaCheck(cudaGetLastError());                   // catches an invalid launch
	}

	void Multiply(const float* a, const float* b, float* out, int n, int k, int m)
	{
		size_t bytesA = size_t(n) * k * sizeof(float);
		size_t bytesB = size_t(k) * m * sizeof(float);
		size_t bytesOut = size_t(n) * m * sizeof(float);
		
		float* dA, * dB, * dOut;

		CudaCheck(cudaMalloc(&dA, bytesA));
		CudaCheck(cudaMalloc(&dB, bytesB));
		CudaCheck(cudaMalloc(&dOut, bytesOut));

		CudaCheck(cudaMemcpy(dA, a, bytesA, cudaMemcpyHostToDevice));
		CudaCheck(cudaMemcpy(dB, b, bytesB, cudaMemcpyHostToDevice));
		
		MultiplyDevice(dA, dB, dOut, n, k, m);

		CudaCheck(cudaMemcpy(out, dOut, bytesOut, cudaMemcpyDeviceToHost));

		CudaCheck(cudaFree(dA));
		CudaCheck(cudaFree(dB));
		CudaCheck(cudaFree(dOut));
	}

}