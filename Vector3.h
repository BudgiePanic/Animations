#pragma once

#ifndef _H_VEC3_
#define _H_VEC3_

struct f3 {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float v[3];
	};
	inline f3() : x(0.0f), y(0.0f), z(0.0f) {}
	inline f3(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}
	inline f3(const float* floatArr) : x(floatArr[0]), y(floatArr[1]), z(floatArr[2]) {}
	inline f3(const float a) : v{a,a,a} {}
};

f3 operator+(const f3& a, const f3& b);

f3 operator-(const f3& a, const f3& b);

// scale the vector 3 float
f3 operator*(const f3& a, const float s);

f3 operator*(const f3& a, const f3& b);

f3 operator/(const f3& a, const f3& b);

bool operator==(const f3& a, const f3& b);

bool operator!=(const f3& a, const f3& b);

float dot(const f3& a, const f3& b);

// AKA magnitude squared
inline float lengthSquared(const f3& a);

// AKA magnitude
inline float length(const f3& a);
// distance(f3 a, f3 b) length(a-b)

void normalize(const f3& a);

f3 normalized(const f3& a);

float angle(const f3& a, const f3& b); 

/// <summary>
/// Determines the projection distance of 'a' onto 'b'.
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns>A vector representing the projection of 'a' onto 'b'</returns>
f3 project(const f3& a, const f3& b);

/// <summary>
/// Determines the rejection vector resulting from projecting 'a' onto b'
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns>A vector orthoginal to 'b' the points to the tip of 'a', originating from the tip of the projection vector</returns>
f3 reject(const f3& a, const f3& b);

f3 reflect(const f3& a, const f3& b);

inline f3 cross(const f3& a, const f3& b);

/// <summary>
/// Linear Interpolation
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="t"></param>
/// <returns></returns>
inline f3 lerp(const f3& a, const f3& b, const float t);

/// <summary>
/// Spherical Linear Interpolation
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="t"></param>
/// <returns></returns>
f3 slerp(const f3& a, const f3& b, const float t);

/// <summary>
/// Nonlinear Interpolation, a cheap substitute for slerp
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="t"></param>
/// <returns></returns>
f3 nlerp(const f3& a, const f3& b, const float t);

#endif

