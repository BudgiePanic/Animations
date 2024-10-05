#pragma once
#include "Track.h"
#include "../transforms/srt.h"

namespace anim {

	/// <summary>
	/// The SRT track stores the scale, rotation, translation keyframes for one bone for one animation
	/// </summary>
	class SRTtrack {
	protected:
		/// <summary>
		/// Can be either a bone ID or an object ID if the SRT track is being used to animate a non-skeletal entity
		/// </summary>
		unsigned int id;
		TrackVector scale;
		TrackQuaternion rotation;
		TrackVector translation;
	public:
		SRTtrack();
		unsigned int GetID();
		void SetID(unsigned int id);
		TrackVector& GetScaleTrack();
		TrackQuaternion& GetQuaternionTrack();
		TrackVector& GetTranslationTrack();
		/// <summary>
		/// Result is only valid if hasValidTrack returns true
		/// </summary>
		/// <returns></returns>
		float GetStartTime();
		/// <summary>
		/// Result is only valid if hasValidTrack returns true
		/// </summary>
		/// <returns></returns>
		float GetEndTime();
		/// <summary>
		/// Checks if at least track stored within the SRTtrack has more than 1 keyframe
		/// </summary>
		/// <returns></returns>
		bool hasValidTrack();
		/// <summary>
		/// Samples the scale, rotation, and translation tracks that are nested within this SRT track.
		/// </summary>
		/// <param name="referencePose">The reference pose is used if a sub-track cannot be sampled at the given sample time</param>
		/// <param name="time">The time the track is sampled</param>
		/// <param name="isTrackLooping"></param>
		/// <returns></returns>
		transforms::srt Sample(const transforms::srt& referencePose, float time, bool isTrackLooping);
	};
}
