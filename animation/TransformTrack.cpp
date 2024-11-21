#include "TransformTrack.h"
#include <limits>
#define dev_machine_fps 144.0f

namespace anim{

template ISRTtrack<TrackVector, TrackQuaternion>;
template ISRTtrack<QuickTrackVector, QuickTrackQuaternion>;

ISRTtrack<TrackVector, TrackQuaternion>::ISRTtrack() {
	this->id = 0;
}

ISRTtrack<QuickTrackVector, QuickTrackQuaternion>::ISRTtrack() : scale(dev_machine_fps), translation(dev_machine_fps), rotation(dev_machine_fps) {
	this->id = 0;
}

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
unsigned int ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::GetID() { return this->id; }

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
void ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::SetID(unsigned int id) { this->id = id; }

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
VECTORTRACKTYPE& ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::GetScaleTrack() { return this->scale; }

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
QUATERNIONTRACKTYPE& ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::GetQuaternionTrack() { return this->rotation; }

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
VECTORTRACKTYPE& ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::GetTranslationTrack() { return this->translation; }

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
float ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::GetStartTime() {
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

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
float ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::GetEndTime() {
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

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
bool ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::hasValidTrack() {
	bool result = false; 
	result |= this->rotation.Size() > 1;
	result |= this->scale.Size() > 1;
	result |= this->translation.Size() > 1;
	return result;
}

template<typename VECTORTRACKTYPE, typename QUATERNIONTRACKTYPE>
transforms::srt ISRTtrack<VECTORTRACKTYPE, QUATERNIONTRACKTYPE>::Sample(const transforms::srt& referencePose, float time, bool isTrackLooping)
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

QuickSRTtrack ToQuickSRTtrack(SRTtrack& slowTrack) {
	QuickSRTtrack answer;
	answer.SetID(slowTrack.GetID());
	answer.GetTranslationTrack() = ToQuickTrack<f3, 3>(slowTrack.GetTranslationTrack());
	answer.GetQuaternionTrack() = ToQuickTrack<rotation::quaternion, 4>(slowTrack.GetQuaternionTrack());
	answer.GetScaleTrack() = ToQuickTrack<f3, 3>(slowTrack.GetScaleTrack());
	return answer;
}

}
