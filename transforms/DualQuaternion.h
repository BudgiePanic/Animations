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
	
	/// <summary>
	/// left to right application of multiplication 
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <returns></returns>
	DualQuaternion operator*(const DualQuaternion& left, const DualQuaternion& right);

	bool operator==(const DualQuaternion& left, const DualQuaternion& right);

	bool operator!=(const DualQuaternion& left, const DualQuaternion& right);

	float dot(const DualQuaternion& left, const DualQuaternion& right);

	DualQuaternion conjugate(const DualQuaternion& dualQuat);

	DualQuaternion normalized(const DualQuaternion& dualQuat);

	void normalize(DualQuaternion& dualQuat);

	DualQuaternion toDualQuaternion(const srt& transform);

	srt toSRT(const DualQuaternion& dualQuat);

	f3 applyPoint(const DualQuaternion& dualQuat, const f3& point);

	f3 applyVector(const DualQuaternion& dualQuat, const f3& vector);
}
