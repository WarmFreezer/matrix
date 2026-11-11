#pragma once

namespace cuda
{
	void Add(const float* a, const float* b, float* out, int n);

	void AddDevice(const float* dA, const float* dB, float* dOut, int n);

	void Multiply(const float* a, const float* b, float* out, int n, int k, int m);

	void MultiplyDevice(const float* dA, const float* dB, float* dOut, int n, int k, int m);
}