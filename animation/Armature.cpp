#include "Armature.h"
#include "../transforms/srt.h"

namespace anim {

	void Armature::RecalculateInverseBindPose() {
		unsigned int numbBones = bindPose.Size();
		this->inverseBindPose.resize(numbBones);
		for (unsigned int bone = 0; bone < numbBones; bone++) {
			transforms::srt worldBone = this->bindPose.GetWorldTransform(bone);
			this->inverseBindPose[bone] = inverse(transforms::toMatrix(worldBone));
		}
	}

	Armature::Armature() {
	}

	Armature::Armature(const Pose& rest, const Pose& bind, const std::vector<std::string>& names) {
		this->Set(rest, bind, names);
	}

	void Armature::Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names) {
		this->bindPose = bind;
		this->boneNames = names;
		this->restPose = rest;
		this->RecalculateInverseBindPose();
	}

	Pose& Armature::GetBindPose() {
		return this->bindPose;
	}

	Pose& Armature::GetRestPose() {
		return this->restPose;
	}

	std::vector<mat4f>& Armature::GetInverseBindPose() {
		return this->inverseBindPose;
	}

	void Armature::GetInverseBindPose(std::vector<transforms::DualQuaternion>& outputArray) {
		unsigned int numbBones = this->bindPose.Size();
		outputArray.resize(numbBones);
		for (unsigned int boneIndex = 0; boneIndex < numbBones; boneIndex++) {
			transforms::DualQuaternion worldBone = this->bindPose.GetWorldDualQuaternion(boneIndex);
			outputArray[boneIndex] = transforms::conjugate(worldBone);
		}
	}

	std::vector<std::string>& Armature::GetBoneNames() {
		return this->boneNames;
	}

	std::string& Armature::GetBoneName(unsigned int index) {
		return this->boneNames[index];
	}
}
