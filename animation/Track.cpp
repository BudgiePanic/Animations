#include "Track.h"

namespace anim {

	template Track<float, 1>;
	template Track<f3, 3>;
	template Track<rotation::quaternion, 4>;

	namespace trackHelpers {
		inline float Interpolate(float a, float b, float t) {
			return a + ((b - a) * t);
		}
		inline f3 Interpolate(const f3& a, const f3& b, float t) {
			return lerp(a,b,t);
		}
		inline rotation::quaternion Interpolate(const rotation::quaternion& a, const rotation::quaternion& b, float t) {
			// check quaternion neighborhood (see quaternion.h for details)
			if (rotation::dot(a,b) < 0.0f) {
				return rotation::normalized(rotation::mix(a,-b,t));
			}
			return rotation::normalized(rotation::mix(a, b, t));
		}
		// Not sure why the author wanted to code it this way
		// If a different type of frame is added, this nonsense is gonna be duplicated for each new type
		// You'd think this would be put into each templated verion's functions
		// re-validates an interpolated quaternion
		inline float HermitePass(float f) { return f; }
		inline f3 HermitePass(const f3& vec) { return vec; }
		inline rotation::quaternion HermitePass(const rotation::quaternion& q) {
			return rotation::normalized(q);
		}
		inline void NeighborhoodPass(const float& a, float& b) { return; }
		inline void NeighborhoodPass(const f3& a, f3& b) { return; }
		inline void NeighborhoodPass(const rotation::quaternion& a, rotation::quaternion& b) {
			if (rotation::dot(a, b) < 0.0f) {
				b = -b;
			}
		}
	}
	
	template<typename T, int FrameDimension>
	Track<T, FrameDimension>::Track() {
		this->interpolation = Interpolate::Linear;
	}

	template<typename T, int FrameDimension>
	float Track<T, FrameDimension>::GetStartTime() { return this->frames[0].timestamp; }

	template<typename T, int FrameDimension>
	float Track<T, FrameDimension>::GetEndTime() { return this->frames[frames.size() - 1].timestamp; }

	template<typename T, int FrameDimension>
	Frame<FrameDimension>& Track<T, FrameDimension>::operator[](unsigned int frameIndex) { return this->frames[frameIndex]; }

	template<typename T, int FrameDimension>
	int Track<T, FrameDimension>::FrameIndexAt(float time, bool isTrackLooping) {
		unsigned int trackSize = (unsigned int)this->frames.size();
		if (trackSize <= 1) { return -1; }
		if (isTrackLooping) {
			// convert time to lie in range [start, end]
			// if the time is before the start time, it is 'near' the end of the 'last loop'
			// if the time is after the end time, it is 'near' the start of the 'next loop'
			float start = frames[0].timestamp;
			float end = frames[trackSize - 1].timestamp;
			float trackDuration = end - start;
			time = fmodf(time - start, trackDuration);
			time = (time < 0.0f) ? (time + trackDuration) : time;
			time += start;
		} else {
			// not looping, so clip out of bounds time values to nearest valid frame
			if (time <= frames[0].timestamp) { return 0; } 
			if (time >= frames[trackSize - 2].timestamp) { 
				// return 2nd to last frame, because how are you meant to sample between 2 frames when there is no next frame?
				// even if the time means we want the last frame information, we'll end up with Interpolate(last-1,last,1.0);
				// and then the interpolate function will return 'last'
				return (int)trackSize - 2; 
			} 
		}
		// time is now valid
		for (int i = ((int)trackSize - 1); i >= 0; i--) {
			// we are looking for the closest frame that is before timestamp.
			// start searching the largest timestamp frames first
			bool frameBeforeTime = time >= frames[i].timestamp;
			if (frameBeforeTime) {
				return i;
			}
		}
		return -1; // unreachable?
	}

	template<typename T, int FrameSize>
	float Track<T, FrameSize>::ClipTime(float time, bool isTrackLooping) {
		unsigned int trackSize = (unsigned int)frames.size();
		if (trackSize <= 1) {
			// if we have less than two frames, we can't do interpolation
			return 0.0f;
		}
		float start = frames[0].timestamp;
		float end = frames[trackSize - 1].timestamp;
		float trackDuration = end - start;
		if (trackDuration <= 0.0f) { return 0.0f; }
		// not sure why the book author chose to ctrl c, ctrl v the time validation code twice instead of pulling the logic up a method
		if (isTrackLooping) {
			time = fmodf(time - start, trackDuration);
			time = (time < 0.0f) ? (time + trackDuration) : time;
			time += start;
		} else {
			if (time <= start) { time = start; }
			if (time >= end) { time = end; }
		}
		return time;
	}

	template<typename T, int FrameDimension>
	unsigned int Track<T, FrameDimension>::Size() { return this->frames.size(); }

	template<typename T, int FrameDimension>
	void Track<T, FrameDimension>::Resize(unsigned int numbFrames) { this->frames.resize(numbFrames); }

	template<typename T, int FrameDimension>
	Interpolate Track<T, FrameDimension>::GetInterpolationMethod() { return this->interpolation; }

	template<typename T, int FrameDimension>
	void Track<T, FrameDimension>::SetInterpolationMethod(Interpolate method) { this->interpolation = method; }

	template<typename T, int FrameDimension>
	T Track<T, FrameDimension>::Sample(float time, bool isTrackLooping) {
		switch (this->interpolation) {
		case Interpolate::Constant: return this->SampleConstant(time, isTrackLooping);
		case Interpolate::Linear: return this->SampleLinear(time, isTrackLooping);
		case Interpolate::Cubic: return this->SampleCubic(time, isTrackLooping);
		}
		// should be unreachable, we'll return constant sample here just in case
		return this->SampleConstant(time, isTrackLooping);
	}

	template<typename T, int FrameDimension>
	T Track<T, FrameDimension>::SampleHermite(float time, const T& point1, const T& slope1, const T& point2, const T& slope2) {
		// I verified that 'a' here is the same as 'a' in hermite in curve.h by working on pen and paper.
		// It has simply been re-arranged. The same is also true for 'b', 'c', 'd'
		float tt = time * time, ttt = tt*time;
		T point2Copy = point2;
		trackHelpers::NeighborhoodPass(point1, point2Copy);
		float a = 2.0f * ttt - 3.0f * tt + 1.0f;
		float b = -2.0f * ttt + 3.0f * tt;
		float c = ttt - 2.0f * tt + time;
		float d = ttt - tt;
		T result = point1 * a + point2 * b + slope1 * c + slope2 * d;
		return trackHelpers::HermitePass(result);
	}

	template<typename T, int FrameSize>
	T Track<T, FrameSize>::SampleConstant(float time, bool isTrackLooping) {
		int index = this->FrameIndexAt(time, isTrackLooping);
		if (index < 0 || index >= (int)frames.size()) {
			// something weird happened
			return T();
		}
		float* data = &frames[index].value[0];
		return ToType(data);
	}

	template<typename T, int FrameSize>
	T Track<T, FrameSize>::SampleLinear(float time, bool isTrackLooping) {
		int index = this->FrameIndexAt(time, isTrackLooping);
		if (index < 0 || index >= (int)frames.size()-1) { return T(); }
		float frameTime = frames[index].timestamp;
		int next = index + 1;
		float nextFrameTime = frames[next].timestamp;
		float interFramePeriod = nextFrameTime - frameTime;
		if (interFramePeriod <= 0.0f) { /*somethings wrong with the frame data*/ return T(); }
		float sampleTime = this->ClipTime(time, isTrackLooping);
		float t = (sampleTime - frameTime) / interFramePeriod; // t is the % distance covered from frame to frame + 1, range [0..1]
		return trackHelpers::Interpolate(
			ToType(&frames[index].value[0]),
			ToType(&frames[next].value[0]),
			t
		);
	}

	template<typename T, int FrameSize>
	T Track<T, FrameSize>::SampleCubic(float time, bool isTrackLooping) {
		// not sure why the book author didn't pull this logic up into another function instead of ctrl c, ctrl v
		// get the indices of the frames we'll be interpolating between
		int index = this->FrameIndexAt(time, isTrackLooping);
		if (index < 0 || index >= (int)frames.size() - 1) { return T(); }
		// determine the time between the two frames
		float frameTime = frames[index].timestamp;
		int next = index + 1;
		float nextFrameTime = frames[next].timestamp;
		float interFramePeriod = nextFrameTime - frameTime;
		if (interFramePeriod <= 0.0f) { return T(); }
		// validate the sample time
		float sampleTime = this->ClipTime(time, isTrackLooping);
		// convert the sample time to a interpolation percentage
		float t = (sampleTime - frameTime) / interFramePeriod;
		size_t fsize = sizeof(float);
		// get the hermite spline parameters out of the frames
		T point1 = ToType(&frames[index].value[0]);
		/*
		    chosing which tangent to use for the keyframe
			s => sample point
			               _s_
			              /   \		
		out slope (index)*     *(next) in slope
		*/
		// thanks to the author's architecture, we have to memcpy the slopes out of the frames because calling ToType would normalize a quaternion slope, invalidating it
		T slope1;
		memcpy(&slope1, frames[index].out, FrameSize * fsize);
		slope1 = slope1 * interFramePeriod; // scale slope1
		T point2 = ToType(&frames[next].value[0]);
		T slope2;
		memcpy(&slope2, frames[next].in, FrameSize * fsize);
		slope2 = slope2 * interFramePeriod; // scale slope2
		return SampleHermite(t, point1, slope1, point2, slope2);
	}

	template<> float Track<float, 1>::ToType(float* frameDataArray) {
		return frameDataArray[0];
	}

	template<> f3 Track<f3, 3>::ToType(float* frameDataArray) {
		return f3(frameDataArray[0], frameDataArray[1], frameDataArray[2]);
	}

	template<> rotation::quaternion Track<rotation::quaternion, 4>::ToType(float* frameDataArray) {
		return rotation::normalized(rotation::quaternion(
			frameDataArray[0], frameDataArray[1], frameDataArray[2], frameDataArray[3]
		));
	}
}
