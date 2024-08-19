#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Mat3f.h"

/// <summary>
/// A 4 by 4 column major matrix.
/// 
/// AEIM [00][04][08][12]
/// BFJN [01][05][09][13]
/// CGKO [02][06][10][14] 
/// DHLP [03][07][11][15]
/// 
/// f[16] => ABCD | EFGH | IJKL | MNOP 
/// [r0c0][r1c0][r2c0][r3c0] | [r0c1][r1c1][r2c1][r3c1] | [r0c2][r1c2][r2c2][r3c2] | [r0c3][r1c3][r2c3][r3c3]
/// [0]   [1]   [2]   [3]    | [4]   [5]   [6]   [7]    | [8]   [9]   [10]  [11]   | [12]  [13]  [14]  [15]
/// [A]   [B]   [C]   [D]    | [E]   [F]   [G]   [H]    | [I]   [J]   [K]   [L]    | [M]   [N]   [O]   [P]
/// 
/// Logical and physical memory layout don't match. Each 'row' in memory is actually a 'column'
/// 
/// ElementIndex = column * numbColumns + row
/// 
/// </summary>
struct mat4f {
	union {
		float v[16]; // v ==> vector
		struct {
			f4 c0; // Access matrix column vectors
			f4 c1; //
			f4 c2; //
			f4 c3; //
		};
		struct {
			float xx; float xy; float xz; float xw; // "basis vector component notation"
			float yx; float yy; float yz; float yw; // vector + vector component
			float zx; float zy; float zz; float zw;
			float tx; float ty; float tz; float tw;
		};
		struct {
			float r0c0; float r1c0; float r2c0; float r3c0; // access matrix via row column notation
			float r0c1; float r1c1; float r2c1; float r3c1;
			float r0c2; float r1c2; float r2c2; float r3c2;
			float r0c3; float r1c3; float r2c3; float r3c3;
		};
		struct {
			float c0r0; float c0r1; float c0r2; float c0r3; // access matrix via column row notation
			float c1r0; float c1r1; float c1r2; float c1r3;
			float c2r0; float c2r1; float c2r2; float c2r3;
			float c3r0; float c3r1; float c3r2; float c3r3;
		};
	};
	// identity matrix initializer
	#pragma warning(disable:26495)
	inline mat4f() :	r0c0(1.0f), r1c0(0.0f), r2c0(0.0f), r3c0(0.0f), 
						r0c1(0.0f), r1c1(1.0f), r2c1(0.0f), r3c1(0.0f),
						r0c2(0.0f), r1c2(0.0f), r2c2(1.0f), r3c2(0.0f),
						r0c3(0.0f), r1c3(0.0f), r2c3(0.0f), r3c3(1.0f) {}
	// copy constructor from float array
	#pragma warning(disable:26495)
	inline mat4f(const float* arr) :	xx(arr[0]),  xy(arr[1]),  xz(arr[2]),  xw(arr[3]),
										yx(arr[4]),  yy(arr[5]),  yz(arr[6]),  yw(arr[7]),
										zx(arr[8]),  zy(arr[9]), zz(arr[10]), zw(arr[11]),
										tx(arr[12]), ty(arr[13]), tz(arr[14]), tw(arr[15]) {}
	// hand specified matrix constructor
	#pragma warning(disable:26495)
	inline mat4f(
		float _00, float _01, float _02, float _03, 
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23,
		float _30, float _31, float _32, float _33
	) :							xx(_00), xy(_01), xz(_02), xw(_03),
								yx(_10), yy(_11), yz(_12), yw(_13),
								zx(_20), zy(_21), zz(_22), zw(_23),
								tx(_30), ty(_31), tz(_32), tw(_33) {}

	// hand specified matrix constructor
	/*#pragma warning(disable:26495) !! How is the compiler supposed to know which initializer to call?
	inline mat4f(
		float r0c0, float r0c1, float r0c2, float r0c3,
		float r1c0, float r1c1, float r1c2, float r1c3,
		float r2c0, float r2c1, float r2c2, float r2c3,
		float r3c0, float r3c1, float r3c2, float r3c3
	) : v { r0c0, r1c0, r2c0, r3c0, r0c1, r1c1, r2c1, r3c1, r0c2, r1c2, r2c2, r3c2, r0c3, r1c3, r2c3, r3c3 } {}*/


};

bool operator==(const mat4f& a, const mat4f& b);

bool operator!=(const mat4f& a, const mat4f& b);

mat4f operator+(const mat4f& a, const mat4f& b);

mat4f operator*(const mat4f& a, float f);

mat4f operator*(const mat4f& a, const mat4f& b);

f4 operator*(const mat4f& a, const f4& b);

constexpr float p = 1.0f;
constexpr float v = 0.0f;

f3 multiplyVector(const mat4f& matrix, const f3& vector);

f3 multiplyPoint(const mat4f& matrix, const f3& point);

f3 multiply(const mat4f& matrix, const f3& tuple, float& w);

void transpose(mat4f& m);

mat4f transposed(const mat4f& m);

float determinant(const mat4f& m);

mat4f inverse(const mat4f& m);

float cofactor(const mat4f& m, int row, int col);

float minor(const mat4f& m, int row, int col);

Mat3f submatrix(const mat4f& m, int row, int col);

void invert(mat4f& m);

mat4f perspective(float fov, float aspect, float near, float far);

mat4f orthographic(float left, float right, float bottom, float top, float near, float far);

mat4f lookAt(const f3& position, const f3& target, const f3& up);

mat4f frustum(float left, float right, float bottom, float top, float near, float far);
