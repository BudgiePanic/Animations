#include "TransformTrack.h"
#include <limits>

namespace anim{

SRTtrack::SRTtrack() {
	this->id = 0;
}

unsigned int SRTtrack::GetID() { return this->id; }

void SRTtrack::SetID(unsigned int id) { this->id = id; }

TrackVector& SRTtrack::GetScaleTrack() { return this->scale; }

TrackQuaternion& SRTtrack::GetQuaternionTrack() { return this->rotation; }

TrackVector& SRTtrack::GetTranslationTrack() { return this->translation; }

float SRTtrack::GetStartTime() {
	static_assert(std::numeric_limits<float>::is_iec559, "No IEEE 754 :(");
	float scaleStart = std::numeric_limits<float>::infinity();
	float rotationStart = scaleStart;
	float translationStart = scaleStart;
	if (this->scale.Size() > 1) {
		scaleStart = this->scale.GetStartTime();
	}
	if (this->rotation.Size() > 1) {
		rotationStart = this->rotation.GetStartTime();
	}
	if (this->translation.Size() > 1) {
		translationStart = this->translation.GetStartTime();
	}
	float scaleRotationSmallest = (scaleStart < rotationStart ? scaleStart : rotationStart);
	return translationStart < scaleRotationSmallest ? translationStart : scaleRotationSmallest;
}

float SRTtrack::GetEndTime() {
	static_assert(std::numeric_limits<float>::is_iec559, "No IEEE 754 :(");
	float scaleEnd = -1.0f * std::numeric_limits<float>::infinity();
	float rotationEnd = scaleEnd;
	float translationEnd = scaleEnd;
	if (this->scale.Size() > 1) {
		scaleEnd = this->scale.GetEndTime();
	}
	if (this->rotation.Size() > 1) {
		rotationEnd = this->rotation.GetEndTime();
	}
	if (this->translation.Size() > 1) {
		translationEnd = this->translation.GetEndTime();
	}
	float scaleRotationLargest = (scaleEnd > rotationEnd ? scaleEnd : rotationEnd);
	return translationEnd > scaleRotationLargest ? translationEnd : scaleRotationLargest;
}

bool SRTtrack::hasValidTrack() {
	bool result = false; 
	result |= this->rotation.Size() > 1;
	result |= this->scale.Size() > 1;
	result |= this->translation.Size() > 1;
	return result;
}

transforms::srt SRTtrack::Sample(const transforms::srt& referencePose, float time, bool isTrackLooping)
{
	transforms::srt result = referencePose; // make a copy of the reference pose
	if (this->translation.Size() > 1) {
		result.position = this->translation.Sample(time, isTrackLooping);
	}
	if (this->rotation.Size() > 1) {
		result.rotation = this->rotation.Sample(time, isTrackLooping);
	}
	if (this->scale.Size() > 1) {
		result.scale = this->scale.Sample(time, isTrackLooping);
	}
	return result;
}

}
