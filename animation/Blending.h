#pragma once
#include "Pose.h"

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


}
