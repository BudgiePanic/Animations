#ifndef _H_QUAT_
#define _H_QUAT_

#include "../Vector3.h"
#include "../Mat4f.h"

namespace rotation {
	struct quaternion {
		union
		{
			struct {
				float x; // i
				float y; // j
				float z; // k 
				float w; // _
			};
			struct {
				f3 vector;
				float scalar;
			};
			float v[4];
		};
		inline quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		inline quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	};

	/// <summary>
	/// Create a quaternion that represents an rotation about an axis.
	/// </summary>
	/// <param name="angle">An angle in radians. +ve angle = CCW rotation</param>
	/// <param name="axis">Rotation axis</param>
	/// <returns></returns>
	quaternion angleAxis(float angle, const f3& axis);

	/// <summary>
	/// Creates a quaternion representing a rotation that goes from vector 'from' to vector 'to'.
	/// </summary>
	/// <param name="from">the start vector</param>
	/// <param name="to">the end vector</param>
	/// <returns>a rotation between 'from' and 'to' vectors</returns>
	quaternion fromTo(const f3& from, const f3& to);

	/// <summary>
	/// Retrieve the rotation axis from a quaternion.
	/// </summary>
	/// <param name="quat">The quaternion to get the rotation axis from</param>
	/// <returns>The axis of rotation of the quaternion</returns>
	f3 getAxis(const quaternion& quat);

	/// <summary>
	/// Retrieve the rotation angle from a quaternion.
	/// </summary>
	/// <param name="quat">The quaternion to get the rotation angle from</param>
	/// <returns>The rotation angle of the quaternion in radians</returns>
	float getAngle(const quaternion& quat);

	// COMPONENT WISE QUATERNION OPERATIONS
	
	// Add two rotations together
	quaternion operator+(const quaternion& a, const quaternion& b);
	// Subtract a rotation from another rotation
	quaternion operator-(const quaternion& a, const quaternion& b);
	// negate a rotation
	quaternion operator-(const quaternion& a);
	// scale a rotation
	quaternion operator*(const quaternion& quat, float scalar);
	// concatinate rotation a to rotation b. i.e. b rotation occurs first, then rotation a is applied.
	quaternion operator*(const quaternion& a, const quaternion& b);

	bool operator==(const quaternion& a, const quaternion& b);
	bool operator!=(const quaternion& a, const quaternion& b);
	/// <summary>
	/// Test if two quaternions represent the same rotation.
	/// Two different quaternions can represent the same rotation by taking the clockwise and Counter clockwise paths
	/// to the same end point.
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns>True if quaternions 'a' and 'b' represent the same rotation</returns>
	bool sameOrientation(const quaternion& a, const quaternion& b);

	/// <summary>
	/// Calculate the dot product between two quaternions.
	/// The dot product is a measure of their similarities.
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	float dot(const quaternion& a, const quaternion& b);

	/// <summary>
	/// Calculate the squared length of a quaternion
	/// </summary>
	/// <param name="quat"></param>
	/// <returns></returns>
	float lengthSquared(const quaternion& quat);

	/// <summary>
	/// Calculate the length of a quaternion
	/// </summary>
	/// <param name="quat"></param>
	/// <returns></returns>
	float length(const quaternion& quat);

	/// <summary>
	/// Normalize a quaternion
	/// </summary>
	/// <param name="quat">A reference to the quaternion to normalize</param>
	void normalize(quaternion& quat);

	/// <summary>
	/// Create a normalized copy of a quaternion
	/// </summary>
	/// <param name="quat">A reference to the quaternion to be normalized</param>
	/// <returns>a normalized copy of the quaternion</returns>
	quaternion normalized(const quaternion& quat);

	/// <summary>
	/// Calculate the conjugate of a quaternion.
	/// If the quaternion is normalized, then the conjugate is also the inverse of the quaternion
	/// The conjugate flips the axis of rotation.
	/// </summary>
	/// <param name="quat"></param>
	/// <returns></returns>
	quaternion conjugate(const quaternion& quat);

	/// <summary>
	/// Calculates the proper inverse of a quaternion.
	/// </summary>
	/// <param name="quat"></param>
	/// <returns></returns>
	quaternion inverse(const quaternion& quat);

	/// <summary>
	/// Check if a quaternion is normalized
	/// </summary>
	/// <param name="quat"></param>
	/// <returns></returns>
	bool isNormalized(const quaternion& quat);

	/// <summary>
	/// Check if a quaternion is pure.
	/// A quaternion is pure when the 'w' component is zero and the quaternion is normalized.
	/// </summary>
	/// <param name="quat"></param>
	/// <returns></returns>
	bool isPure(const quaternion& quat);

	// transform a vector by a rotation encoded in the quaternion
	f3 operator*(const quaternion& rotation, const f3& vector);

	/*
	  The problem of 'neighborhooding' in quaternions:
	  As the quaternion encodes a rotation we can rotate along the shortest arc along the unit circle to the target rotation
	  Or we can rotate along the longest arc. How to decide? Check the dot product of the two quaternions being 
	  interpolated between. A positive dot product means the shortest arc will be taken.
	  Taking the shortest path is usually desirable. If the dot product is negative, then one of the quaternions should be 
	  negated, this will cause the interpolation of rotations to take the shortest path.
	*/

	// LERP NLERP SLERP and MIX

	/// <summary>
	/// Mix two quaternions together.
	/// If input quaternions 'a' and 'b' are both normalized, the result of this function is the same as LERP.
	/// Assumes 'from' and 'to' are in the desired neighborhood already.
	/// </summary>
	/// <param name="from"></param>
	/// <param name="to"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	quaternion mix(const quaternion& from, const quaternion& to, float t);

	/// <summary>
	/// A fast approximation of slerp.
	/// Assumes 'from' and 'to' are in the desired neighborhood already.
	/// </summary>
	/// <param name="from"></param>
	/// <param name="to"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	quaternion nlerp(const quaternion& from, const quaternion& to, float t);

	/// <summary>
	/// Interpolates between two rotations with a consistent velocity.
	/// </summary>
	/// <param name="from"></param>
	/// <param name="to"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	quaternion slerp(const quaternion& from, const quaternion& to, float t);

	/// <summary>
	/// Raise a quaternion to a power.
	/// Adjusts the rotation angle of a quaternion.
	/// For example raising a quaternion to 0.5 will change the roation angle to be half of what it was originally.
	/// </summary>
	/// <param name="quat"></param>
	/// <param name="power"></param>
	/// <returns></returns>
	quaternion operator^(const quaternion& quat, float power);

	/// <summary>
	/// Create a quaternion that rotates to 'look' in a direction.
	/// The rotation of a lookRotation quaternion is the inverse of a camera's lookAt matrix rotation, for the same look direction.
	/// </summary>
	/// <param name="direction">The direction to look towards</param>
	/// <param name="up">THe up direction</param>
	/// <returns></returns>
	quaternion lookRotation(const f3& direction, const f3& up);

	// MATRIX QUATERNION INTEROP FUNCTIONS
	// Used to convert quaternion data into something the GPU can use.

	/// <summary>
	/// Convert the rotation encoded in a quaternion into a rotation matrix.
	/// </summary>
	/// <param name="quat"></param>
	/// <returns></returns>
	mat4f toMatrix(const quaternion& quat);

	/// <summary>
	/// Convert the rotation encoded in a rotation matrix into a quaternion.
	/// </summary>
	/// <param name="matrix"></param>
	/// <returns></returns>
	quaternion toQuaternion(const mat4f& matrix);
}

#endif
