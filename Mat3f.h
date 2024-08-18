#pragma once
#include "Mat2f.h"

struct Mat3f {
	union {
		struct
		{
			float r0c0; float r0c1; float r0c2;
			float r1c0; float r1c1; float r1c2;
			float r2c0; float r2c1; float r2c2;
		};
		float v[9];
	};
	inline Mat3f(float _r0c0, float _r0c1, float _r0c2, float _r1c0, float _r1c1, float _r1c2, float _r2c0, float _r2c1, float _r2c2) : 
	r0c0(_r0c0), r0c1(_r0c1), r0c2(_r0c2), r1c0(_r1c0), r1c1(_r1c1), r1c2(_r1c2), r2c0(_r2c0), r2c1(_r2c1), r2c2(_r2c2) {}
};

float determinant(const Mat3f& m);

Mat2::Mat2f submatrix(const Mat3f& m, int row, int col);

float minor(const Mat3f& m, int row, int col);

float cofactor(const Mat3f& m, int row, int col);
