#pragma once

/// <summary>
/// Data container struct for passing data to the GPU (Hence no accompanying math operations)
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
struct TVec4 {
	union {
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		T v[4];
	};
	inline TVec4<T>() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}				// blank initializer
	inline TVec4<T>(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}	// value initializer
	inline TVec4<T>(const T* other) : x(other[0]), y(other[1]), z(other[2]), w(other[3]) {}			// copy initializer
};

typedef TVec4<float> f4;
typedef TVec4<int> i4;
