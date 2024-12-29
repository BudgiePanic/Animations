#include "Pose.h"

namespace anim {

	Pose::Pose() {}

	Pose::Pose(unsigned int numbBones) {
		this->Resize(numbBones);
	}

	Pose::Pose(const Pose& pose) {
		*this = pose;
	}

	/// <summary>
	/// Create a deep copy of a pose when assigning
	/// </summary>
	/// <param name="pose"></param>
	/// <returns></returns>
	Pose& Pose::operator=(const Pose& pose)
	{
		if (&pose == this) {
			return *this;
		}
		if (this->boneParents.size() != pose.boneParents.size()) {
			this->boneParents.resize(pose.boneParents.size());
		}
		if (this->bones.size() != pose.bones.size()) {
			this->bones.resize(pose.bones.size());
		}
		if (this->boneParents.size() != 0) {
			memcpy(&this->boneParents[0], &pose.boneParents[0], sizeof(int) * this->boneParents.size());
		}
		if (this->bones.size() != 0) {
			memcpy(&this->bones[0], &pose.bones[0], sizeof(transforms::srt) * this->bones.size());
		}
		return *this;
	}

	void Pose::Resize(unsigned int numbBones) {
		this->boneParents.resize(numbBones);
		this->bones.resize(numbBones);
	}

	unsigned int Pose::Size() {
		return this->bones.size();
	}

	int Pose::ParentIndexOf(unsigned int boneIndex)	{
		return this->boneParents[boneIndex];
	}

	void Pose::SetParentIndex(unsigned int boneIndex, unsigned int parentIndex)	{
		this->boneParents[boneIndex] = parentIndex;
	}

	transforms::srt Pose::GetLocalTransform(unsigned int boneIndex) {
		return this->bones[boneIndex];
	}

	void Pose::SetLocalTransform(unsigned int boneIndex, const transforms::srt& localTransform)	{
		this->bones[boneIndex] = localTransform;
	}

	transforms::srt Pose::GetWorldTransform(unsigned int boneIndex)	{
		// make a copy of the local transform
		transforms::srt result = this->bones[boneIndex];
		// follow the bone hierarchy
		int parentIndex = this->boneParents[boneIndex];
		while (parentIndex >= 0) {
			result = transforms::combine(this->bones[parentIndex], result);
			parentIndex = this->boneParents[parentIndex];
		}
		return result;
	}

	transforms::srt Pose::operator[](unsigned int boneIndex) {
		return GetWorldTransform(boneIndex);
	}

	void Pose::ToMatrixPalette(std::vector<mat4f>& outputArray)	{
		unsigned int numbBones = this->Size();
		if (outputArray.size() != numbBones) {
			outputArray.resize(numbBones);
		}
		// try to use the optimized caching method, which relies on parent bones being at a lower index than child bones
		unsigned int bone = 0;
		for (; bone < numbBones; bone++) {
			int parentOfBone = this->boneParents[bone];
			if (parentOfBone > bone) { break; } // can't use the optimized method anymore, bones aren't in ascending order
			mat4f boneAsMatrix = transforms::toMatrix(this->bones[bone]); // local bone space
			if (parentOfBone >= 0) {
				boneAsMatrix = outputArray[parentOfBone] * boneAsMatrix; // converted to model space
			}
			outputArray[bone] = boneAsMatrix;
		}
		// revert to naive implementation if the bones are not stored in ascending order
		for (; bone < numbBones; bone++) {
			transforms::srt transform = this->GetWorldTransform(bone);
			outputArray[bone] = transforms::toMatrix(transform);
		}
	}

	bool Pose::operator==(const Pose& other) {
		if (this->bones.size() != other.bones.size()) { return false; }
		if (this->boneParents.size() != other.boneParents.size()) { return false; }
		unsigned int numbBones = this->Size();
		for (unsigned int i = 0; i < numbBones; i++) {
			transforms::srt ours = this->bones[i];
			transforms::srt theirs = other.bones[i];
			if (ours.position != theirs.position ||
				ours.rotation != theirs.rotation ||
				ours.scale != theirs.scale) { 
				return false;
			}
			if (this->boneParents[i] != other.boneParents[i]) { return false; }
		}
		return true;
	}

	bool Pose::operator!=(const Pose& other) {
		return !(*this == other);
	}
}
