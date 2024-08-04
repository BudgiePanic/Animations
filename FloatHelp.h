#pragma once

#include <Math.h>

constexpr int AisBigger = 1;
constexpr int BisBigger = -1;
constexpr int AequalsB = 0;

// The maximum difference between floats before they are considered the same
constexpr float epsilon = 0.0001f;

// Currently does not handle cases: a || b is INF && a || b is NaN
inline int FloatCompare(const float a, const float b) {
	const float delta = fabsf(a - b);
	if (delta < epsilon) { return AequalsB; }
	if (a < b) { return BisBigger; }
	return AisBigger;
}
