#pragma once

#include "../rotation/quaternion.h"
#include "srt.h"

namespace transforms {
	/// <summary>
	/// A dual quaternion encodes position and rotation (but not scale) data in a single, efficient, data structure
	/// </summary>
	struct DualQuaternion {
		union {
			struct {
				/// <summary>
				/// Rotational data
				/// </summary>
				rotation::quaternion real;
				/// <summary>
				/// positional data
				/// </summary>
				rotation::quaternion dual;
			};
			float v[8];
		};
		DualQuaternion() : real(0.0f,0.0f,0.0f,1.0f), dual(0.0f,0.0f,0.0f,0.0f) {}
		DualQuaternion(const rotation::quaternion _real, const rotation::quaternion _dual) : real(_real), dual(_dual) {}
	};

	DualQuaternion operator+(const DualQuaternion& left, const DualQuaternion& right);
	DualQuaternion operator*(const DualQuaternion& dualQuat, float scale);
}
