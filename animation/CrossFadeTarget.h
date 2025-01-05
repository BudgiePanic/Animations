#pragma once
#include "Pose.h"
#include "Clip.h"

namespace anim {

	/// <summary>
	/// Data courier containing information about the animation that is being faded into from another currently playing animation
	/// Used in animation crossfading
	/// </summary>
	struct CrossFadeTarget {
		Pose pose;
		// The animation clip that is being faded into
		Clip* clip;
		float time;
		float duration;
		float elapsed;
		CrossFadeTarget() : clip(NULL), time(0.0f), duration(0.0f), elapsed(0.0f) {}
		CrossFadeTarget(Clip* target, Pose& pose, float dur) : clip(target), time(target->GetStartTime()), duration(dur), elapsed(0.0f) {}
	};

}
