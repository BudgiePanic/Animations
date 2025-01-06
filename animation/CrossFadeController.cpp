#include "CrossFadeController.h"
#include "Blending.h"

namespace anim {

	CrossFadeController::CrossFadeController() {
		this->clip = NULL;
		this->time = 0.0f;
		this->IsArmatureSet = false;
	}

	CrossFadeController::CrossFadeController(Armature& armature) {
		this->clip = NULL;
		this->time = 0.0f;
		this->SetArmature(armature);
	}

	void CrossFadeController::SetArmature(Armature& armature) {
		this->IsArmatureSet = true;
		this->armature = armature;
		this->pose = this->armature.GetRestPose();
	}

	void CrossFadeController::Play(Clip* target) {
		this->targets.clear();
		this->clip = target;
		this->pose = this->armature.GetRestPose();
		this->time = this->clip->GetStartTime();
	}

	void CrossFadeController::FadeTo(Clip* target, float time) {
		if (this->clip == NULL) {
			// no clip currently playing, so just play this clip
			this->Play(target);
			return;
		}
		if ((this->targets.size() >= 1) && (this->targets.back().clip == target)) {
			// book author doesn't explain why we can't add the same clip to the target list multiple times in a row
			return;
		}
		if (this->targets.empty() && this->clip == target) {
			// requested animation clip is already playing
			return; 
		} 
		this->targets.push_back(CrossFadeTarget(target, this->armature.GetRestPose(), time));
	}

	void CrossFadeController::Update(float elapsedTime) {
		if (this->clip == NULL || !this->IsArmatureSet) {
			// no clip playing, nothing to update OR no bones to pose
			return;
		}
		// remove a fade target if it has finished 'fading'
		unsigned int numbTargets = this->targets.size();
		for (unsigned int i = 0; i < numbTargets; i++) {
			CrossFadeTarget& target = this->targets[i];
			if (target.elapsed >= target.duration) {
				this->clip = target.clip;
				this->time = target.time;
				this->pose = target.pose;
				this->targets.erase(this->targets.begin() + i);
				break;
			}
		}
		this->pose = this->armature.GetRestPose();
		this->time = this->clip->Sample(this->pose, this->time + elapsedTime);
		numbTargets = this->targets.size(); // a target may have been deleted in the above for loop
		for (unsigned int i = 0; i < numbTargets; i++) {
			CrossFadeTarget& target = this->targets[i];
			target.time = target.clip->Sample(target.pose, target.time + elapsedTime);
			target.elapsed += elapsedTime;
			float percentage = target.elapsed / target.duration;
			percentage = percentage > 1.0f ? 1.0f : percentage;
			Blend(this->pose, this->pose, target.pose, percentage, -1);
		}
	}

	Pose& CrossFadeController::GetCurrentPose()	{
		return this->pose;
	}

	Clip* CrossFadeController::GetCurrentClip()	{
		return this->clip;
	}

}
