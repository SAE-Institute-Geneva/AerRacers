#include <benchmark/benchmark.h>
#include <mathematics/transform.h>
#include <mathematics/quaternion.h>

namespace neko
{
const long fromRange = 8;
const long toRange = 1 << 15;

static void BM_RotationMatrix(benchmark::State& state)
{
	for (auto _ : state)
	{
		for (long i = 0; i < state.range(0); i++)
		{
			benchmark::DoNotOptimize(
					Vec3f(Transform3d::RotationMatrixFrom(degree_t(90.0f), Vec3f::left) * Vec4f(Vec3f::one)));
		}
	}
}

BENCHMARK(BM_RotationMatrix)->Range(fromRange, toRange);

static void BM_RotationQuaternion(benchmark::State& state)
{
	for (auto _ : state)
	{
		for (long i = 0; i < state.range(0); i++)
		{
			benchmark::DoNotOptimize(
					Quaternion::AngleAxis(degree_t(90.0f), Vec3f::left) * Vec3f::one);
		}
	}
}

BENCHMARK(BM_RotationQuaternion)->Range(fromRange, toRange);
}
BENCHMARK_MAIN();
