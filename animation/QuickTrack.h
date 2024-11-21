#pragma once
#include "Track.h"

namespace anim {

	/// <summary>
	/// Fast track uses caching to speed up the search for the nearest animation frame when sampling the track
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="FrameDimension"></typeparam>
	template<typename T, int FrameDimension>
	class QuickTrack : public Track<T, FrameDimension> {
	protected:
		std::vector<unsigned int> nearestFrameIndices;
		virtual int FrameIndexAt(float time, bool isTrackLooping);
		float expectedFramesPerSecond;
	public:
		void RecalculateFrameIndexCache();
		QuickTrack(float expectedFramesPerSecond);
	};

	typedef QuickTrack<float, 1> QuickTrackScalar;
	typedef QuickTrack<f3, 3> QuickTrackVector;
	typedef QuickTrack<rotation::quaternion, 4> QuickTrackQuaternion;

	/// <summary>
	/// Converts a track using the original track implementation into a quick track for faster sampling.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="FrameDimension"></typeparam>
	/// <param name="slowTrack"></param>
	/// <returns></returns>
	template<typename T, int FrameDimension>
	QuickTrack<T, FrameDimension> ToQuickTrack(Track<T, FrameDimension>& slowTrack);

}
