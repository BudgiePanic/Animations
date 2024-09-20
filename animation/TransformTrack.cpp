#include "TransformTrack.h"

namespace anim{

SRTtrack::SRTtrack() {
	this->id = 0;
}

unsigned int SRTtrack::GetID() { return this->id; }

void SRTtrack::SetID(unsigned int id) { this->id = id; }

TrackScalar& SRTtrack::GetScaleTrack() { return this->scale; }

TrackQuaternion& SRTtrack::GetQuaternionTrack() { return this->rotation; }

TrackVector& SRTtrack::GetTranslationTrack() { return this->translation; }

bool SRTtrack::hasValidTrack() {
	bool result = false; 
	result |= this->rotation.Size() > 1;
	result |= this->scale.Size() > 1;
	result |= this->translation.Size() > 1;
	return result;
}

}
