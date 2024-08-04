#pragma once

/// <summary>
/// Data container struct for passing data to the GPU (Hence no accompanying math operations)
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
struct TVec2 {
	union {
		struct {
			T x;
			T y;
		};
		T v[2];
	};
	inline TVec2<T>() : x(T(0)), y(T(0)) {}							// blank initializer
	inline TVec2<T>(T _x, T _y) : x(_x), y(_y) {}					// value initializer
	inline TVec2<T>(const T* other) : x(other[0]), y(other[1]) {}	// copy initializer
};

typedef TVec2<float> f2;
typedef TVec2<int> i2;