#include "FABRIKSolver.h"

namespace ik {

	void FABRIKSolver::ToChainPosition() {

	}

	void FABRIKSolver::Forward(const f3& target) {

	}

	void FABRIKSolver::Backward(const f3& rootBone) {

	}

	FABRIKSolver::FABRIKSolver() {

	}

	unsigned int FABRIKSolver::ChainSize() {
		return 0;
	}

	void FABRIKSolver::ResizeChain(unsigned int size) {
	}

	unsigned int FABRIKSolver::GetIterMaxSteps() {
		return 0;
	}

	void FABRIKSolver::SetIterMaxSteps(unsigned int numbSteps) {

	}

	float FABRIKSolver::GetSolveThreshold() {
		return 0.0f;
	}

	void FABRIKSolver::SetSolveThreshold(float threshold) {

	}

	transforms::srt FABRIKSolver::GetLocalBone(unsigned int boneIndex) {
		return transforms::srt();
	}

	void FABRIKSolver::SetLocalBone(unsigned int boneIndex, const transforms::srt& bone) {

	}

	transforms::srt FABRIKSolver::GetBone(unsigned int boneIndex) {
		return transforms::srt();
	}

	bool FABRIKSolver::SolveChain(const transforms::srt& target) {
		return false;
	}

}
