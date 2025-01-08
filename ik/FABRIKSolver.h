#pragma once

#include <vector>
#include "../transforms/srt.h"

namespace ik {

	/// <summary>
	/// Forward And Backward Reaching Inverse Kinematic Iterative Solver.
	/// Solves single linear bone chains.
	/// </summary>
	class FABRIKSolver {
	protected:
		/// <summary>
		/// The inverse kinematic bone chain is stored in hierarchical order.
		/// So boneChain[n] is a child of boneChain[n-1] and so on until boneChain[0].
		/// Each bone in boneChain is stored in local bone space. 
		/// The chain's end effector is stored at boneChain[size()-1].
		/// The solver needs 2 or more bones to work correctly.
		/// The chain is modified in place during solving. 
		/// </summary>
		std::vector<transforms::srt> localBoneChain;

		/// <summary>
		/// The model space bone chain, but as positions. Local model space.
		/// FABRIK uses position data instead of rotation, so we need a second copy of the chain.
		/// </summary>
		std::vector<f3> bonePosChain;

		/// <summary>
		/// The length from the bone origin to the origin of the child bone.
		/// </summary>
		std::vector<float> boneLengths;

		/// <summary>
		/// The maximum number of iteration steps before the solver gives up.
		/// </summary>
		unsigned int iterMaxSteps;
		
		/// <summary>
		/// If the end effector of the localBoneChain gets within this threshold distance of the target
		/// the solver will accept the current solution and return early.
		/// </summary>
		float solveThreshold;
	protected:

		/// <summary>
		/// Converts the local bone space chain to model space positions and writes the result to bonePosChain.
		/// </summary>
		void ToChainPosition();

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="target"></param>
		void Forward(const f3& target);

		/// <summary>
		/// TODO
		/// </summary>
		/// <param name="rootBone"></param>
		void Backward(const f3& rootBone);

		/// <summary>
		/// Converts the model space bone position chain back into local bone space SRT chain.
		/// </summary>
		void ChainPositionToLocalBone();

	public:
		// boiler plate

		/// <summary>
		/// Create a FABRIK solver with default iteration steps and solution threshold.
		/// </summary>
		FABRIKSolver();

		/// <summary>
		/// Get the number of bones in the IK chain that this solver is using
		/// </summary>
		/// <returns>The number of bones in the solver's IK chain</returns>
		unsigned int ChainSize();

		/// <summary>
		/// Change the amount of memory allocated to store bones in the solver's IK chain
		/// </summary>
		/// <param name="size">The new number of bones to allocate memory for</param>
		void ResizeChain(unsigned int size);

		/// <summary>
		/// Get the number of iteration steps this IK solver is using.
		/// </summary>
		/// <returns>The number of iteration steps being used by the solver</returns>
		unsigned int GetIterMaxSteps();

		/// <summary>
		/// Set the number of iteration steps this IK solver should use
		/// </summary>
		/// <param name="numbSteps">The new number of iteration steps</param>
		void SetIterMaxSteps(unsigned int numbSteps);

		/// <summary>
		/// Get the distance the solver uses to determine if the end effector is close enough to the target
		/// </summary>
		/// <returns></returns>
		float GetSolveThreshold();

		/// <summary>
		/// Change the distance the solver uses to determine if the end effector is close enough to the target
		/// </summary>
		/// <param name="threshold">the new distance to determine when the end effector is close enough to the target</param>
		void SetSolveThreshold(float threshold);

		/// <summary>
		/// Get a bone in local bone space.
		/// </summary>
		/// <param name="boneIndex"></param>
		/// <returns></returns>
		transforms::srt GetLocalBone(unsigned int boneIndex);

		/// <summary>
		/// Set the local bone at an index.
		/// </summary>
		/// <param name="boneIndex"></param>
		/// <param name="bone"></param>
		void SetLocalBone(unsigned int boneIndex, const transforms::srt& bone);

		/// <summary>
		/// Get a bone in local model space.
		/// </summary>
		/// <param name="boneIndex"></param>
		/// <returns></returns>
		transforms::srt GetBone(unsigned int boneIndex);

		/// <summary>
		/// Run the IK solver to rotate the IK chain's bones so that the end effector reaches a target.
		/// Solving the chain will modify the solver's bone chain in-place.
		/// </summary>
		/// <param name="target">The model space location that the IK solver should try to move the end effector to</param>
		/// <returns>True if the end effector was able to reach the target</returns>
		bool SolveChain(const transforms::srt& target);
	};
}
