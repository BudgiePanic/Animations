#pragma once
#include "../Vector3.h"
#include "../rotation/quaternion.h"
#include "../Mat4f.h"

namespace transforms {
	/// <summary>
	/// Scale, Rotation, Translation struct.
	/// Used instead of a Mat4f transform matrix.
	/// Both accomplish the same goal, but SRTs can be interpolated between, transform matrices cannot.
	/// </summary>
	struct srt {
		f3 position;
		rotation::quaternion rotation;
		f3 scale;

		srt(const f3& _position, const rotation::quaternion& _rotation, const f3& _scale) : 
		position(_position), rotation(_rotation), scale(_scale) {}

		srt() : position(f3(0,0,0)), rotation(rotation::quaternion(0,0,0,1)), scale(f3(1,1,1)) {}
	};

	/// <summary>
	/// Combine the effects (scale, rotation, translation) of two transforms together into a single SRT.
	/// Combines in a right to left order, similar to matrix multiplication.
	/// </summary>
	/// <param name="a">The child transform</param>
	/// <param name="b">The parent transform</param>
	/// <returns></returns>
	srt combine(const srt& a, const srt& b);

	/// <summary>
	/// Create an SRT that reverses the coordinate space mapping of the SRT.
	/// </summary>
	/// <param name="transform"></param>
	/// <returns></returns>
	srt inverse(const srt& transform);

	/// <summary>
	/// Interpolate between two transforms.
	/// </summary>
	/// <param name="from"></param>
	/// <param name="to"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	srt mix(const srt& from, const srt& to, float t);

	/// <summary>
	/// Convert SRT to a transform matrix. Matrix transform representation works better on the GPU.
	/// </summary>
	/// <param name="transform"></param>
	/// <returns></returns>
	mat4f toMatrix(const srt& transform);

	/// <summary>
	/// Convert a transformation matrix to SRT.
	/// Useful when parsing animation data from fileformats that store poses as matrices.
	/// The conversion is lossy, the scale information may be inaccurate due to skew also being encoded in the matrix.
	/// </summary>
	/// <param name="transform"></param>
	/// <returns></returns>
	srt toSRT(const mat4f& transform);

	/// <summary>
	/// Apply the transform to a point. Creates a new point in the coordinate space of the SRT.
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="point"></param>
	/// <returns></returns>
	f3 applyPoint(const srt& transform, const f3& point);

	/// <summary>
	/// Apply the transform to a vector. Creates a new vector in the coordinate space of the SRT.
	/// </summary>
	/// <param name="transform"></param>
	/// <param name="vector"></param>
	/// <returns></returns>
	f3 applyVector(const srt& transform, const f3& vector);

}
