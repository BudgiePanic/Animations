#pragma once
#include <vector>
#include "CrossFadeTarget.h"
#include "Pose.h"
#include "Armature.h"

namespace anim {
/// <summary>
/// Managers the fading of animations onto a playing animation clip
/// </summary>
class CrossFadeController {
protected:
	std::vector<CrossFadeTarget> targets;
	Clip* clip;
	float time;
	Pose pose; 
	Armature armature;
	bool IsArmatureSet;
public:
	CrossFadeController();
	CrossFadeController(Armature& armature);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="armature"></param>
	void SetArmature(Armature& armature);
	/// <summary>
	/// Set the current animation to play back
	/// </summary>
	/// <param name="target"></param>
	void Play(Clip* target);
	/// <summary>
	/// Blend to a new animation
	/// </summary>
	/// <param name="target"></param>
	/// <param name="time"></param>
	void FadeTo(Clip* target, float time);
	/// <summary>
	/// Resample animation clips
	/// </summary>
	/// <param name="elapsedTime">Time since the last sample</param>
	void Update(float elapsedTime);
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Pose& GetCurrentPose();
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	Clip* GetCurrentClip();
};
}
