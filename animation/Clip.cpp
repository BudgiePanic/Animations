#include "Clip.h"

namespace anim {

	template IClip<SRTtrack>;
	template IClip<QuickSRTtrack>;

	template<typename TRACKIMPLTYPE>
	float IClip<TRACKIMPLTYPE>::ClipTime(float time) {
		if (this->doesClipLoop) {
			float clipDuration = endTime - startTime;
			if (clipDuration <= 0.0f) { return 0.0f; } // invalid clip data
			time = fmodf(time - startTime, clipDuration);
			time = (time < 0.0f) ? time + clipDuration : time;
			time += startTime;
		} else {
			if (time < startTime) { time = this->startTime; }
			if (time > endTime) { time = this->endTime; }
		}
		return time;
	}

	template<typename TRACKIMPLTYPE>
	IClip<TRACKIMPLTYPE>::IClip() {
		this->clipName = "Unnamed animation clip";
		this->startTime = 0.0f;
		this->endTime = 0.0f;
		this->doesClipLoop = true;
	}

	template<typename TRACKIMPLTYPE>
	unsigned int IClip<TRACKIMPLTYPE>::Size() {
		return (unsigned int) this->tracks.size();
	}

	template<typename TRACKIMPLTYPE>
	unsigned int IClip<TRACKIMPLTYPE>::GetTrackBoneIDAtIndex(unsigned int index) {
		return this->tracks[index].GetID();
	}

	template<typename TRACKIMPLTYPE>
	void IClip<TRACKIMPLTYPE>::SetTrackBoneID(unsigned int trackIndex, unsigned int boneID)	{
		this->tracks[trackIndex].SetID(boneID);
	}

	template<typename TRACKIMPLTYPE>
	TRACKIMPLTYPE& IClip<TRACKIMPLTYPE>::operator[](unsigned int boneID)	{
		int numbTracks = tracks.size();
		for (int track = 0; track < numbTracks; track++) {
			if (tracks[track].GetID() == boneID) {
				return tracks[track];
			}
		}
		tracks.push_back(TRACKIMPLTYPE());
		tracks.back().SetID(boneID);
		return tracks.back();
	}

	template<typename TRACKTYPEIMPL>
	float IClip<TRACKTYPEIMPL>::Sample(Pose& pose, float time)
	{
		if (this->GetDuration() == 0.0f) { return 0.0f; }
		time = this->ClipTime(time);
		unsigned int numbTracks = this->tracks.size();
		for (unsigned int track = 0; track < numbTracks; track++) {
			unsigned int boneIndex = tracks[track].GetID();
			transforms::srt localTransform = pose.GetLocalTransform(boneIndex);
			transforms::srt animatedTransform = tracks[track].Sample(localTransform, time, this->doesClipLoop);
			pose.SetLocalTransform(boneIndex, animatedTransform);
		}
		return time;
	}

	template<typename TRACKTYPEIMPL>
	void IClip<TRACKTYPEIMPL>::CalculateClipDuration() {
		this->startTime = 0.0f;
		this->endTime = 0.0f;
		bool foundStartTime = false;
		bool foundEndTime = false;
		unsigned int numbTracks = this->tracks.size();
		for (unsigned int track = 0; track < numbTracks; track++) {
			if (tracks[track].hasValidTrack()) {
				float trackStart = tracks[track].GetStartTime();
				float trackEnd = tracks[track].GetEndTime();
				if (!foundStartTime || trackStart < startTime) {
					startTime = trackStart;
					foundStartTime = true;
				}
				if (!foundEndTime || trackEnd > endTime) {
					endTime = trackEnd;
					foundEndTime = true;
				}
			}
		}
	}

	template<typename TRACKTYPEIMPL>
	std::string& IClip<TRACKTYPEIMPL>::GetClipName() {
		return this->clipName;
	}

	template<typename TRACKTYPEIMPL>
	void IClip<TRACKTYPEIMPL>::SetClipName(std::string& name) {
		this->clipName = name;
	}

	template<typename TRACKTYPEIMPL>
	float IClip<TRACKTYPEIMPL>::GetDuration() {
		return this->endTime - this->startTime;
	}

	template<typename TRACKTYPEIMPL>
	float IClip<TRACKTYPEIMPL>::GetStartTime() {
		return this->startTime;
	}

	template<typename TRACKTYPEIMPL>
	float IClip<TRACKTYPEIMPL>::GetEndTime() {
		return this->endTime;
	}

	template<typename TRACKTYPEIMPL>
	bool IClip<TRACKTYPEIMPL>::DoesClipLoop() {
		return this->doesClipLoop;
	}

	template<typename TRACKTYPEIMPL>
	void IClip<TRACKTYPEIMPL>::SetClipLooping(bool doesClipLoop) {
		this->doesClipLoop = doesClipLoop;
	}

	QuickClip ToQuickClip(Clip& slowClip) {
		QuickClip answer;
		answer.SetClipName(slowClip.GetClipName());
		answer.SetClipLooping(slowClip.DoesClipLoop());
		unsigned int numbBones = slowClip.Size();
		for (unsigned int i = 0; i < numbBones; i++) {
			unsigned int bone = slowClip.GetTrackBoneIDAtIndex(i);
			answer[bone] = ToQuickSRTtrack(slowClip[bone]);
		}
		answer.CalculateClipDuration();
		return answer;
	}

}
