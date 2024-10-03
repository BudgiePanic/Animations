#include "Pose.h"

namespace anim {

	Pose::Pose() {}

	Pose::Pose(unsigned int numbBones) {
		this->Resize(numbBones);
	}

	Pose::Pose(const Pose& pose) {
		*this = pose;
	}

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
			memcpy(&this->bones[0], &pose.bones[0], sizeof(int) * this->bones.size());
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
		// naive implementation, not currently using caching
		for (unsigned int bone = 0; bone < numbBones; bone++) {
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
