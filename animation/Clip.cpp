#include "Clip.h"

namespace anim {

	float Clip::ClipTime(float time) {
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

	Clip::Clip() {
		this->clipName = "Unnamed animation clip";
		this->startTime = 0.0f;
		this->endTime = 0.0f;
		this->doesClipLoop = true;
	}

	unsigned int Clip::Size() {
		return (unsigned int) this->tracks.size();
	}

	unsigned int Clip::GetTrackBoneIDAtIndex(unsigned int index) {
		return this->tracks[index].GetID();
	}

	void Clip::SetTrackBoneID(unsigned int trackIndex, unsigned int boneID)	{
		this->tracks[trackIndex].SetID(boneID);
	}

	SRTtrack& Clip::operator[](unsigned int boneID)	{
		int numbTracks = tracks.size();
		for (int track = 0; track < numbTracks; track++) {
			if (tracks[track].GetID() == boneID) {
				return tracks[track];
			}
		}
		tracks.push_back(SRTtrack());
		tracks.back().SetID(boneID);
		return tracks.back();
	}

	float Clip::Sample(Pose& pose, float time)
	{
		if (this->GetDuration() == 0.0f) { return 0.0f; }
		time = this->ClipTime(time);
		unsigned int numbTracks = this->tracks.size();
		for (int track = 0; track < numbTracks; track++) {
			unsigned int boneIndex = tracks[track].GetID();
			transforms::srt localTransform = pose.GetLocalTransform(boneIndex);
			transforms::srt animatedTransform = tracks[track].Sample(localTransform, time, this->doesClipLoop);
			pose.SetLocalTransform(boneIndex, animatedTransform);
		}
		return time;
	}

	void Clip::CalculateClipDuration() {
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

	std::string& Clip::GetClipName() {
		return this->clipName;
	}

	void Clip::SetClipName(std::string& name) {
		this->clipName = name;
	}

	float Clip::GetDuration() {
		return this->endTime - this->startTime;
	}

	float Clip::GetStartTime() {
		return this->startTime;
	}

	float Clip::GetEndTime() {
		return this->endTime;
	}

	bool Clip::DoesClipLoop() {
		return this->doesClipLoop;
	}

	void Clip::SetClipLooping(bool doesClipLoop) {
		this->doesClipLoop = doesClipLoop;
	}

}
