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
		int numbBones =  (int) this->ChainSize();
		int effectorIndex = numbBones - 1;
		if (numbBones > 1) {
			/* The solver needs 2 or more bones to work properly. One bone is not a bone chain */
			// move the end effector to the target positon
			this->bonePosChain[effectorIndex] = target;
		} 
		// move the rest of the bones along with the end effector, maintain their distances
		for (int boneIndex = effectorIndex - 1; boneIndex >= 0; boneIndex--) {
			f3 toChild = normalized(this->bonePosChain[boneIndex] - this->bonePosChain[boneIndex + 1]);
			f3 offset = toChild * this->boneLengths[boneIndex + 1];
			this->bonePosChain[boneIndex] = this->bonePosChain[boneIndex + 1] + offset;
		}
	}

	void FABRIKSolver::Backward(const f3& rootBone) {
		unsigned int numbBones = this->ChainSize();
		if (numbBones > 0) {
			this->bonePosChain[0] = rootBone;
		}
		for (unsigned int boneIndex = 1; boneIndex < numbBones; boneIndex++) {
			f3 toParent = normalized(this->bonePosChain[boneIndex] - this->bonePosChain[boneIndex - 1]);
			f3 offset = toParent * this->boneLengths[boneIndex];
			this->bonePosChain[boneIndex] = this->bonePosChain[boneIndex - 1] + offset;
		}
		// If a solution was found, the end effector will still be touching the target when this function completes :)
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
		unsigned int numbBones = this->ChainSize();
		if (numbBones < 2) { return false; /* not enough bones to run the algorithm */ }

		unsigned int effectorIndex = numbBones - 1;
		float threshSq = this->solveThreshold * this->solveThreshold;

		this->ToChainPosition();
		f3 targetPos = target.position;
		f3 rootBonePos = this->bonePosChain[0];

		for (unsigned int iter = 0; iter < this->iterMaxSteps; iter++) {
			f3 currEffectorPos = this->bonePosChain[effectorIndex];
			if (lengthSquared(targetPos - currEffectorPos) < threshSq) {
				this->ChainPositionToLocalBone();
				return true;
			}
			this->Forward(targetPos);
			this->Backward(rootBonePos);
		}

		this->ChainPositionToLocalBone();
		f3 effectorPos = this->GetBone(effectorIndex).position;
		return lengthSquared(targetPos - effectorPos) < threshSq;
	}

}
