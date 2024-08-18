#pragma once

namespace Mat2 {
	struct Mat2f
	{
		float r0c0; float r0c1;
		float r1c0; float r1c1;
		inline Mat2f(float* f4) : r0c0(f4[0]), r0c1(f4[1]), r1c0(f4[2]), r1c1(f4[3]) {}
		inline Mat2f(float _r0c0, float _r0c1, float _r1c0, float _r1c1) : r0c0(_r0c0), r0c1(_r0c1), r1c0(_r1c0), r1c1(_r1c1) {}
	};

	float determinant(const Mat2f& mat);
}
