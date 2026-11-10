// Each test compares Matrix against a plain double-precision reference.
// Tests print their name before running so a crash still shows which one died.

#include "Matrix.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using matrix::Matrix;

static int failures = 0;

#define CHECK(cond) \
	do { if (!(cond)) { std::cout << "    FAIL line " << __LINE__ << ": " #cond "\n"; failures++; } } while (0)

#define CHECK_THROWS(expr) \
	do { bool threw = false; try { (void)(expr); } catch (const std::invalid_argument&) { threw = true; } \
	     if (!threw) { std::cout << "    FAIL line " << __LINE__ << ": expected throw: " #expr "\n"; failures++; } } while (0)

// Row-major double copy, the reference every result is checked against
using Ref = std::vector<double>;

static std::mt19937 rng(12345);

static Matrix Random(int rows, int cols)
{
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	Matrix m(rows, cols);
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			m[r][c] = dist(rng);
	return m;
}

static Ref ToRef(const Matrix& m)
{
	Ref out;
	for (int r = 0; r < m.Rows(); r++)
		for (int c = 0; c < m.Cols(); c++)
			out.push_back(m[r][c]);
	return out;
}

// float sums lose precision as k grows, so the tolerance scales with the summed length
static bool Near(const Matrix& actual, const Ref& expected, int rows, int cols, int k = 1)
{
	if (actual.Rows() != rows || actual.Cols() != cols)
	{
		std::cout << "    shape " << actual.Rows() << "x" << actual.Cols() << ", expected " << rows << "x" << cols << "\n";
		return false;
	}
	double tol = 1e-5 + 1e-6 * k;
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
		{
			double e = expected[r * cols + c];
			if (std::abs(actual[r][c] - e) > tol * std::max(1.0, std::abs(e)))
			{
				std::cout << "    [" << r << "][" << c << "] = " << actual[r][c] << ", expected " << e << "\n";
				return false;
			}
		}
	return true;
}

static Ref RefMul(const Ref& a, const Ref& b, int n, int k, int m)
{
	Ref out(n * m, 0.0);
	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			for (int x = 0; x < k; x++)
				out[i * m + j] += a[i * k + x] * b[x * m + j];
	return out;
}

static Ref RefMap(const Ref& a, const std::function<double(double)>& f)
{
	Ref out(a.size());
	for (size_t i = 0; i < a.size(); i++) out[i] = f(a[i]);
	return out;
}

static Ref RefZip(const Ref& a, const Ref& b, const std::function<double(double, double)>& f)
{
	Ref out(a.size());
	for (size_t i = 0; i < a.size(); i++) out[i] = f(a[i], b[i]);
	return out;
}

// ---- construction and assignment ----

static void TestConstruction()
{
	Matrix m(3, 4);
	CHECK(m.Rows() == 3 && m.Cols() == 4);
	CHECK(Near(m, Ref(12, 0.0), 3, 4));
	CHECK_THROWS(Matrix(0, 4));
	CHECK_THROWS(Matrix(3, -1));
}

static void TestCopyAndMove()
{
	Matrix a = Random(3, 5);
	Ref ra = ToRef(a);

	Matrix copy(a);
	copy[0][0] += 1.0f;
	CHECK(Near(a, ra, 3, 5)); // copy is independent of the original

	Matrix assigned;
	assigned = a;
	CHECK(Near(assigned, ra, 3, 5));

	Matrix moved(std::move(copy));
	CHECK(moved.Rows() == 3 && moved.Cols() == 5);
	CHECK(copy.GetMatrix() == nullptr);

	Matrix moveAssigned;
	moveAssigned = std::move(assigned);
	CHECK(Near(moveAssigned, ra, 3, 5));
	CHECK(assigned.GetMatrix() == nullptr);

	a = a; // self-assignment must keep the data
	CHECK(Near(a, ra, 3, 5));
}

static void TestTranspose()
{
	Matrix a = Random(3, 7);
	Ref ra = ToRef(a);
	Ref expected(21);
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 7; c++)
			expected[c * 3 + r] = ra[r * 7 + c];
	CHECK(Near(Matrix::Transpose(a), expected, 7, 3));
}

// ---- element-wise operators ----

static void TestAddSubtract()
{
	Matrix a = Random(4, 6), b = Random(4, 6);
	Ref ra = ToRef(a), rb = ToRef(b);

	CHECK(Near(a + b, RefZip(ra, rb, [](double x, double y) { return x + y; }), 4, 6));
	CHECK(Near(a - b, RefZip(ra, rb, [](double x, double y) { return x - y; }), 4, 6));
	CHECK(Near(a % b, RefZip(ra, rb, [](double x, double y) { return x * y; }), 4, 6));

	CHECK_THROWS(a + Random(6, 4));
	CHECK_THROWS(a - Random(6, 4));
	CHECK_THROWS(a % Random(6, 4));
}

static void TestScalar()
{
	Matrix a = Random(5, 3);
	Ref ra = ToRef(a);
	const float s = 2.5f;

	CHECK(Near(a + s, RefMap(ra, [&](double x) { return x + s; }), 5, 3));
	CHECK(Near(s + a, RefMap(ra, [&](double x) { return s + x; }), 5, 3));
	CHECK(Near(a - s, RefMap(ra, [&](double x) { return x - s; }), 5, 3));
	CHECK(Near(s - a, RefMap(ra, [&](double x) { return s - x; }), 5, 3));
	CHECK(Near(a * s, RefMap(ra, [&](double x) { return x * s; }), 5, 3));
	CHECK(Near(s * a, RefMap(ra, [&](double x) { return s * x; }), 5, 3));
}

// A 1x1 matrix on either side acts like a scalar
static void TestOneByOneBroadcast()
{
	Matrix a = Random(3, 4);
	Matrix one(1, 1);
	one[0][0] = 2.0f;
	Ref ra = ToRef(a);

	CHECK(Near(a + one, RefMap(ra, [](double x) { return x + 2.0; }), 3, 4));
	CHECK(Near(one + a, RefMap(ra, [](double x) { return 2.0 + x; }), 3, 4));
	CHECK(Near(a - one, RefMap(ra, [](double x) { return x - 2.0; }), 3, 4));
	CHECK(Near(one - a, RefMap(ra, [](double x) { return 2.0 - x; }), 3, 4));
}

// ---- matrix multiply ----

static void TestMultiply(int n, int k, int m)
{
	Matrix a = Random(n, k), b = Random(k, m);
	CHECK(Near(a * b, RefMul(ToRef(a), ToRef(b), n, k, m), n, m, k));
}

static void TestMultiplySizes()
{
	TestMultiply(1, 1, 1);
	TestMultiply(3, 5, 4);     // non-square
	TestMultiply(1, 64, 1);    // row times column
	TestMultiply(64, 1, 64);   // column times row
	TestMultiply(64, 64, 64);
	TestMultiply(257, 129, 65); // sizes that don't divide evenly into GPU tiles
	TestMultiply(512, 512, 512);

	CHECK_THROWS(Random(3, 4) * Random(5, 3));
}

static void TestMultiplyIdentity()
{
	Matrix a = Random(16, 16);
	Matrix id(16, 16);
	for (int i = 0; i < 16; i++) id[i][i] = 1.0f;
	CHECK(Near(a * id, ToRef(a), 16, 16));
	CHECK(Near(id * a, ToRef(a), 16, 16));
}

// ---- in-place operators: the left-hand matrix itself must change ----

static void TestInPlace()
{
	Matrix a = Random(4, 6), b = Random(4, 6);
	Ref ra = ToRef(a), rb = ToRef(b);

	Matrix x = a;
	x += b;
	CHECK(Near(x, RefZip(ra, rb, [](double p, double q) { return p + q; }), 4, 6));

	x = a;
	x -= b;
	CHECK(Near(x, RefZip(ra, rb, [](double p, double q) { return p - q; }), 4, 6));

	x = a;
	x += 1.5f;
	CHECK(Near(x, RefMap(ra, [](double p) { return p + 1.5; }), 4, 6));

	x = a;
	x -= 1.5f;
	CHECK(Near(x, RefMap(ra, [](double p) { return p - 1.5; }), 4, 6));

	x = a;
	x *= 3.0f;
	CHECK(Near(x, RefMap(ra, [](double p) { return p * 3.0; }), 4, 6));

	Matrix c = Random(6, 5);
	x = a;
	x *= c;
	CHECK(Near(x, RefMul(ra, ToRef(c), 4, 6, 5), 4, 5, 6));
}

int main()
{
	const std::pair<const char*, void (*)()> tests[] = {
		{ "Construction", TestConstruction },
		{ "CopyAndMove", TestCopyAndMove },
		{ "Transpose", TestTranspose },
		{ "AddSubtract", TestAddSubtract },
		{ "Scalar", TestScalar },
		{ "OneByOneBroadcast", TestOneByOneBroadcast },
		{ "MultiplySizes", TestMultiplySizes },
		{ "MultiplyIdentity", TestMultiplyIdentity },
		{ "InPlace", TestInPlace },
	};

	for (const auto& [name, test] : tests)
	{
		std::cout << name << "\n" << std::flush;
		int before = failures;
		try { test(); }
		catch (const std::exception& e) { std::cout << "    FAIL unexpected exception: " << e.what() << "\n"; failures++; }
		std::cout << (failures == before ? "    ok\n" : "");
	}

	std::cout << (failures == 0 ? "\nAll tests passed\n" : "\n" + std::to_string(failures) + " check(s) failed\n");
	return failures == 0 ? 0 : 1;
}
