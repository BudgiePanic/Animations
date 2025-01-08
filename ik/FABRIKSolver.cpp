#include "FABRIKSolver.h"

namespace ik {

	void FABRIKSolver::ToChainPosition() {

	}

	void FABRIKSolver::Forward(const f3& target) {

	}

	void FABRIKSolver::Backward(const f3& rootBone) {

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
