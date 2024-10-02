#pragma once
#include <vector>
#include <string>
#include "TransformTrack.h"
#include "Pose.h"

namespace anim{

	/// <summary>
	/// Animation Clip stores a collection of animation tracks.
	/// </summary>
	class Clip {
	protected:
		/// <summary>
		/// Each entry in tracks describes the change of one bone over time.
		/// </summary>
		std::vector<SRTtrack> tracks;
		std::string clipName;
		float startTime;
		float endTime;
		bool doesTrackLoop;
	protected:
		/// <summary>
		/// Converts timestamps outside the animation clip's valid range into valid time stamps.
		/// </summary>
		/// <param name="time"></param>
		/// <returns></returns>
		float ClipTime(float time);
	public:
		Clip();
		/// <summary>
		/// Get the number of bones this animation clip is supposed to work with
		/// </summary>
		/// <returns>The number of bones this track is using</returns>
		unsigned int Size();
		/// <summary>
		/// Get the Bone ID that is influenced by the animation track stored at a specific index.
		/// </summary>
		/// <param name="index">The index of the animation track in the clip's track vector</param>
		/// <returns></returns>
		unsigned int GetTrackBoneIDAtIndex(unsigned int index);
		/// <summary>
		/// Change the bone that is influenced by an animation track stored in this clip's track vector.
		/// </summary>
		/// <param name="trackIndex">The index of the animation track</param>
		/// <param name="boneID">The ID of the bone that the track influences</param>
		void SetTrackBoneID(unsigned int trackIndex, unsigned int boneID);
		/// <summary>
		/// Returns the animation track for a given bone ID.
		/// If no track exists for the bone ID, one is created and returned.
		/// </summary>
		/// <param name="boneID"></param>
		/// <returns></returns>
		SRTtrack& operator[](unsigned int boneID);
		/// <summary>
		/// Samples the animation clip and writes the resulting pose into pose.
		/// Does nothing if the clip contains no valid animation data.
		/// The provided sample time will be remapped to be in the clip's valid time range.
		/// </summary>
		/// <param name="pose">The pose that the sample is written to</param>
		/// <param name="time">The time at which the clip should be sampled</param>
		/// <returns>The time that was actually used to sample the clip.</returns>
		float Sample(Pose& pose, float time);
		void CalculateClipDuration();
		std::string& GetClipName();
		void SetClipName(std::string& name);
		float GetDuration();
		float GetStartTime();
		float GetEndTime();
		bool DoesClipLoop();
		void SetClipLooping(bool doesClipLoop);
	};
}
