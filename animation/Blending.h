#pragma once
#include "Pose.h"
#include "Armature.h"
#include "Clip.h"

/// <summary>
/// Functions for blending between animations (poses)
/// </summary>

namespace anim {

	/// <summary>
	/// Checks if a bone is a child of another bone in a pose bone hierarchy
	/// </summary>
	/// <param name="pose"></param>
	/// <param name="parentBone"></param>
	/// <param name="boneToCheck"></param>
	/// <returns>true if boneToCheck is a child of parentBone in the pose bone hierarchy</returns>
	bool IsBoneChildOf(Pose& pose, unsigned int parentBone, unsigned int boneToCheck);

	/// <summary>
	/// Combine two poses together. Assumes the bone hierarchy in each pose is the same.
	/// </summary>
	/// <param name="poseOut">Where the resulting blended pose should be written to</param>
	/// <param name="a">the base pose</param>
	/// <param name="b">the pose that is blended onto the base pose</param>
	/// <param name="t">percentage amount of pose 'b' to include. i.e. 0 implies all pose 'a', no pose 'b'</param>
	/// <param name="rootBone">The starting bone of pose B which is blended into pose 'a'. i.e. left arm to only apply blend to an arm</param>
	void Blend(Pose& poseOut, Pose& a, Pose& b, float t, int rootBone);

	/// <summary>
	/// Samples the clip at time zero into a new pose.
	/// </summary>
	/// <param name="armature"></param>
	/// <param name="clip"></param>
	/// <returns>A pose used as a reference when adding two poses together in the AddToPose function.</returns>
	Pose MakePoseForAdding(Armature& armature, Clip& clip);

	/// <summary>
	/// Add's one pose to another. i.e. adding a lean left animation onto a walk animation. 
	/// </summary>
	/// <param name="out">Where the resulting added pose should be written to</param>
	/// <param name="in">The pose that will be added to</param>
	/// <param name="poseToAdd">The pose to add onto the 'in' pose</param>
	/// <param name="baseAddPose">TODO</param>
	/// <param name="rootBone">The starting bone of poseToAdd which is added onto the 'in' Pose. i.e. left arm to only add from the left arm downwards</param>
	void AddToPose(Pose& out, Pose& in, Pose& poseToAdd, Pose& baseAddPose, int rootBone);
}
