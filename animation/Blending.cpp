#include "Blending.h"

namespace anim {

	bool IsBoneChildOf(Pose& pose, unsigned int parentBone, unsigned int boneToCheck) {
		if (parentBone == boneToCheck) { return true; }
		int parent = pose.ParentIndexOf(boneToCheck);
		while (parent >= 0) {
			if (parent == (int) parentBone) { return true; }
			parent = pose.ParentIndexOf(parent);
		}
		return false;
	}

	void Blend(Pose& poseOut, Pose& a, Pose& b, float t, int rootBone) {
		unsigned int numbBones = poseOut.Size();
		for (unsigned int bone = 0; bone < numbBones; bone++) {
			// negative root bone is assume to mean the caller wants blending applied to every bone
			if (rootBone >= 0 && !IsBoneChildOf(poseOut, (unsigned int) rootBone, bone)) { continue; }
			// blend bones in local pose space
			poseOut.SetLocalTransform(bone,
				transforms::mix(a.GetLocalTransform(bone), b.GetLocalTransform(bone), t)
			);
		}
	}

	Pose MakePoseForAdding(Armature& armature, Clip& clip) {
		Pose copy = armature.GetRestPose();
		clip.Sample(copy, clip.GetStartTime());
		return copy;
	}

	void AddToPose(Pose& out, Pose& in, Pose& poseToAdd, Pose& baseAddPose, int rootBone) {
		// how to add pose:
		// (add pose delta) = add pose - add pose base
		// resulting pose = input pose + (add pose delta)
		unsigned int numbBones = poseToAdd.Size();
		for (int bone = 0; bone < (int)numbBones; bone++) {
			if (rootBone < 0 || IsBoneChildOf(poseToAdd, rootBone, bone)) {
				transforms::srt input = in.GetLocalTransform(bone);
				transforms::srt add = poseToAdd.GetLocalTransform(bone);
				transforms::srt addBase = baseAddPose.GetLocalTransform(bone);
				transforms::srt result(
					input.position + (add.position - addBase.position),
					normalized(input.rotation * (inverse(addBase.rotation) * add.rotation)),
					input.scale + (add.scale - addBase.scale)
				);
				out.SetLocalTransform(bone, result);
			}
		}
	}
}
