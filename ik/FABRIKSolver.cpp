#include "FABRIKSolver.h"

namespace ik {

	void FABRIKSolver::ToChainPosition() {
		unsigned int numbBones = this->ChainSize();
		for (unsigned int boneIndex = 0; boneIndex < numbBones; boneIndex++) {
			transforms::srt bone = this->GetBone(boneIndex);
			this->bonePosChain[boneIndex] = bone.position;
			if (boneIndex > 0) {
				f3 previous = this->bonePosChain[boneIndex - 1];
				this->boneLengths[boneIndex] = length(bone.position - previous);
			}
		}
		if (numbBones > 0) {
			this->boneLengths[0] = 0.0f;
		}
	}

	void FABRIKSolver::Forward(const f3& target) {

	}

	void FABRIKSolver::Backward(const f3& rootBone) {

	}

	void FABRIKSolver::ChainPositionToLocalBone() {
		unsigned int numbBones = this->ChainSize();
		if (numbBones < 1) { return; }
		for (unsigned int boneIndex = 0; boneIndex < numbBones - 1; boneIndex++) {
			// skip last bone because modifying the end effector's rotation has no effect on reaching the goal
			transforms::srt bone = GetBone(boneIndex);
			transforms::srt childBone = GetBone(boneIndex + 1);
			
			f3 position = bone.position;
			rotation::quaternion rotation = bone.rotation;

			// vector pointing from current bone to child bone
			f3 toChild = childBone.position - position;
			// convert vector to local bone space
			toChild = rotation::inverse(rotation) * toChild;
			// vector that points from child bone to current position in local model space
			f3 toDesiredPosition = this->bonePosChain[boneIndex + 1] - position;
			// converted to local bone space
			toDesiredPosition = rotation::inverse(rotation) * toDesiredPosition;
			// we want the vector pointing to the child bone to be the same as the vector pointing to the desired position
			// so we create a rotation that will achieve this
			rotation::quaternion difference = rotation::fromTo(toChild, toDesiredPosition);
			// add this rotation to the bone
			this->localBoneChain[boneIndex].rotation = difference * this->localBoneChain[boneIndex].rotation;
		}
	}

	FABRIKSolver::FABRIKSolver() {
		constexpr unsigned int defaultIterCount = 15;
		constexpr float defaultThreshold = 0.0001f;
		this->iterMaxSteps = defaultIterCount;
		this->solveThreshold = defaultThreshold;
	}

	unsigned int FABRIKSolver::ChainSize() { return this->localBoneChain.size(); }

	void FABRIKSolver::ResizeChain(unsigned int size) {
		this->localBoneChain.resize(size);
		this->bonePosChain.resize(size);
		this->boneLengths.resize(size);
	}

	unsigned int FABRIKSolver::GetIterMaxSteps() { return this->iterMaxSteps; }

	void FABRIKSolver::SetIterMaxSteps(unsigned int numbSteps) {
		this->iterMaxSteps = numbSteps;
	}

	float FABRIKSolver::GetSolveThreshold() { return this->solveThreshold; }

	void FABRIKSolver::SetSolveThreshold(float threshold) {
		this->solveThreshold = threshold;
	}

	transforms::srt FABRIKSolver::GetLocalBone(unsigned int boneIndex) { return this->localBoneChain[boneIndex]; }

	void FABRIKSolver::SetLocalBone(unsigned int boneIndex, const transforms::srt& bone) {
		this->localBoneChain[boneIndex] = bone;
	}

	transforms::srt FABRIKSolver::GetBone(unsigned int boneIndex) {
		// who would've thought, it's the exact same as the CCD implementation
		transforms::srt answer = this->localBoneChain[boneIndex];
		for (int i = ((int)boneIndex) - 1; i >= 0; i--) {
			answer = transforms::combine(this->localBoneChain[i], answer);
		}
		return answer;
	}

	bool FABRIKSolver::SolveChain(const transforms::srt& target) {
		return false;
	}

}
