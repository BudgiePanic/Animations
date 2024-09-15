#pragma once
#include <vector>
#include "Frame.h"
#include "Interpolate.h"
#include "../Vector3.h"
#include "../rotation/quaternion.h"

namespace anim {

	template<typename T, int FrameDimension>
	class Track {
	protected:
		std::vector<Frame<FrameDimension>> frames;
		Interpolate interpolation;
	public:
		Track();
		/// <summary>
		/// Update the tracks internal datastructure to allocate space for the given number of frames
		/// </summary>
		/// <param name="numbFrames">TThe number of frames this track should be able to store</param>
		void Resize(unsigned int numbFrames);
		/// <summary>
		/// Get the number of animation frames in this animation track
		/// </summary>
		/// <returns>The number of animation frames stored in the animation track</returns>
		unsigned int Size();
		/// <summary>
		/// Gets the interpolation method being used to interpolate between frames in this animation track.
		/// </summary>
		/// <returns></returns>
		Interpolate GetInterpolationMethod();
		void SetInterpolationMethod(Interpolate method);
		float GetStartTime();
		float GetEndTime();
		/// <summary>
		/// Sample the animation track
		/// </summary>
		/// <param name="time"></param>
		/// <param name="isTrackLooping"></param>
		/// <returns></returns>
		T Sample(float time, bool isTrackLooping);
		/// <summary>
		/// Overload [] operator to allow indexing into the keyframe list.
		/// </summary>
		/// <param name="frameIndex"></param>
		/// <returns></returns>
		Frame<FrameDimension>& operator[](unsigned int frameIndex);
	protected:
		/* Not sure why the book author wants to do it this way, when we could use dynamic dispatch. At least its easy to understand*/
		/// <summary>
		/// Constant sampling, otherwise called stepwise sampling, returns the value of the last keyframe, until the timestamp passes a new keyframe
		/// </summary>
		/// <param name="time"></param>
		/// <param name="isTrackLooping"></param>
		/// <returns></returns>
		T SampleConstant(float time, bool isTrackLooping);
		T SampleLinear(float time, bool isTrackLooping);
		T SampleCubic(float time, bool isTrackLooping);
		T SampleHermite(float time, const T& point1, const T& slope1, const T& point2, const T& slope2);
		/// <summary>
		/// Returns the frame index of the closest frame that comes before the timestamp.
		/// </summary>
		/// <param name="time"></param>
		/// <param name="isTrackLooping"></param>
		/// <returns></returns>
		int FrameIndexAt(float time, bool isTrackLooping);
		/// <summary>
		/// Converts timestamps outside the track's valid range into valid time stamps.
		/// </summary>
		/// <param name="time"></param>
		/// <param name="isTrackLooping"></param>
		/// <returns></returns>
		float ClipTime(float time, bool isTrackLooping);
		/// <summary>
		/// Converts a frame's data array into the concrete data type the frame represents.
		/// </summary>
		/// <param name="frameArrayValue"></param>
		/// <returns></returns>
		T ToType(float* frameArrayValue);
	};

	typedef Track<float, 1> TrackScalar;
	typedef Track<f3, 3> TrackVector;
	typedef Track<rotation::quaternion, 4> TrackQuaternion;

}
