#include "CCDSolver.h"
#include <cassert>

namespace ik {

	CCDSolver::CCDSolver() {
		constexpr unsigned int defaultIterSteps = 15;
		constexpr float defaultThreshold = 0.0001f;
		this->iterMaxSteps = defaultIterSteps;
		this->solveThreshold = defaultThreshold;
	}

	unsigned int CCDSolver::ChainSize()	{ return this->localBoneChain.size(); }

	void CCDSolver::ResizeChain(unsigned int size) {
		this->localBoneChain.resize(size);
	}

	transforms::srt& CCDSolver::operator[](unsigned int index) {
		assert(this->localBoneChain.size() > index); // catch trying to access an empty IK chain
		return this->localBoneChain[index];
	}

	transforms::srt CCDSolver::GetBoneAt(unsigned int index) {
		// no bounds checking on index, because we're living dangerous
		transforms::srt answer = this->localBoneChain[index]; // (*this)[index]; // does the same thing
		// exploit the parent child relationship encoded in the chain ordering for speed up
		for (int i = ((int)index) - 1; i >= 0; i--) {
			answer = transforms::combine(this->localBoneChain[i], answer);
		}
		return answer;
	}

	unsigned int CCDSolver::GetIterMaxSteps() { return this->iterMaxSteps; }

	void CCDSolver::SetIterMaxSteps(unsigned int numbSteps)	{
		this->iterMaxSteps = numbSteps;
	}

	float CCDSolver::GetSolveThreshold() { return this->solveThreshold; }

	void CCDSolver::SetSolveThreshold(float threshold) {
		this->solveThreshold = threshold;
	}

	bool CCDSolver::SolveChain(const transforms::srt& target) {
		// set up
		unsigned int numbBones = this->ChainSize();
		if (numbBones <= 1) { return false; }
		f3 targetPos = target.position;
		float threshSq = this->solveThreshold * this->solveThreshold;
		unsigned int effectorIndex = numbBones - 1;

		f3 effectorPos = this->GetBoneAt(effectorIndex).position;
		// check if we need to do any work
		if (lengthSquared(targetPos - effectorPos) < threshSq) { return true; }

		for (unsigned int i = 0; i < this->iterMaxSteps; i++) {
			// rotate every bone in the chain, skip the end effector, since rotating the effector has no effect.
			// rotations move the effector towards the target. Check if we can stop after each bone rotation is applied.
			for (int boneIndex = ((int)effectorIndex) - 1; boneIndex >= 0; boneIndex--) {
				// the CCD algorithm
				transforms::srt modelSpaceBone = this->GetBoneAt(boneIndex);
				f3 bonePos = modelSpaceBone.position;
					// effectorPos = this->GetBoneAt(effectorIndex).position; // TODO delete this line if no bugs in demo
				// make a rotation for this bone that points the effector towards the target in model space
				f3 boneToEffector = effectorPos - bonePos;
				f3 boneToTarget = targetPos - bonePos;
				rotation::quaternion effectorToTarget;
				if (lengthSquared(boneToTarget) > threshSq) {
					effectorToTarget = rotation::fromTo(boneToEffector, boneToTarget);
				}
				// add the 'point at target' rotation to the bone's rotation
				rotation::quaternion boneRotation = modelSpaceBone.rotation;
				rotation::quaternion newBoneRotation = boneRotation * effectorToTarget;
				// convert the rotation back into local bone space
				rotation::quaternion newBoneRotationLocal = newBoneRotation * rotation::inverse(boneRotation);

				transforms::srt& bone = this->localBoneChain[boneIndex];
				bone.rotation = newBoneRotationLocal * bone.rotation;
				// update effector model space position after each bone movement
				effectorPos = this->GetBoneAt(effectorIndex).position;
				if (lengthSquared(targetPos - effectorPos) < threshSq) { return true; }
			}
		}
		return false;
	}
}
