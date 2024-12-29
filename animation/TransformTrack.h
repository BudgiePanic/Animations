#pragma once
#include "Track.h"
#include "../transforms/srt.h"
#include "QuickTrack.h"

namespace anim {

	/// <summary>
	/// The ISRT track stores the scale, rotation, translation keyframes for one bone for one animation.
	/// It is templated to have both a legacy track and quick track implementation
	/// </summary>
	/// <typeparam name="VECTORTRACKTYPE">the implementation type of the vector tracks (quick or legacy)</typeparam>
	/// <typeparam name="QUATERNIONTRACKTYPE">the implementation type of the quaterion track (quick or legacy)</typeparam>
	template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
	class ISRTtrack {
	protected:
		/// <summary>
		/// Can be either a bone ID or an object ID if the SRT track is being used to animate a non-skeletal entity
		/// </summary>
		unsigned int id;
		VECTORTRACKTYPE scale;
		QUATERNIONTRACKTYPE rotation;
		VECTORTRACKTYPE translation;
	public:
		ISRTtrack();
		unsigned int GetID();
		void SetID(unsigned int id);
		VECTORTRACKTYPE& GetScaleTrack();
		QUATERNIONTRACKTYPE& GetQuaternionTrack();
		VECTORTRACKTYPE& GetTranslationTrack();
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

	/// <summary>
	/// The legacy srt track type, that uses slow sampling tracks
	/// </summary>
	typedef ISRTtrack<TrackVector, TrackQuaternion> SRTtrack;

	/// <summary>
	/// SRT track that uses a track implementation with a faster sampling algorithm
	/// </summary>
	typedef ISRTtrack<QuickTrackVector, QuickTrackQuaternion> QuickSRTtrack;

	/// <summary>
	/// Copes slowTrack data into a QuickSRTtrack. Expensive, call during program initialization.
	/// </summary>
	/// <param name="slowTrack"></param>
	/// <returns></returns>
	QuickSRTtrack ToQuickSRTtrack(SRTtrack& slowTrack);
}
