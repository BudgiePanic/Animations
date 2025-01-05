#include "Blending.h"

namespace anim {

	bool IsBoneChildOf(Pose& pose, unsigned int parentBone, unsigned int boneToCheck) {
		if (parentBone == boneToCheck) { return true; }
		int parent = pose.ParentIndexOf(boneToCheck);
		while (parent >= 0) {
			if (parent == parentBone) { return true; }
			parent = pose.ParentIndexOf(parent);
		}
		return false;
	}

	void Blend(Pose& poseOut, Pose& a, Pose& b, float t, int rootBone) {
		unsigned int numbBones = poseOut.Size();
		for (unsigned int bone = 0; bone < numbBones; bone++) {
			// negative root bone is assume to mean the caller wants blending applied to every bone
			if (rootBone >= 0 && !IsBoneChildOf(poseOut, rootBone, bone)) { continue; }
			// blend bones in local pose space
			poseOut.SetLocalTransform(bone,
				transforms::mix(a.GetLocalTransform(bone), b.GetLocalTransform(bone), t)
			);
		}
	}
}
