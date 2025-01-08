#pragma once

#include <vector>
#include "../transforms/srt.h"

// FABRIK and CCD are iterative IK solvers, but there are other solvers out there.
// There is also analytical solution methods (plug values into a formula, get an answer, O(1)) and Jacobian Matrix Solvers

namespace ik {

	/// <summary>
	/// Cyclic Coordinate Descent Iterative Inverse Kinematic Solver
	/// </summary>
	class CCDSolver {
	protected:
		/// <summary>
		/// The inverse kinematic bone chain is stored in hierarchical order.
		/// So boneChain[n] is a child of boneChain[n-1] and so on until boneChain[0].
		/// Each bone in boneChain is stored in local bone space. 
		/// The chain's end effector is stored at boneChain[size()-1].
		/// The solver needs 2 of more bones to work correctly.
		/// The chain is modified in place during solving. 
		/// </summary>
		std::vector<transforms::srt> localBoneChain;
		/// <summary>
		/// The maximum number of iteration steps before the solver gives up.
		/// </summary>
		unsigned int iterMaxSteps;
		/// <summary>
		/// If the end effector of the localBoneChain gets within this threshold distance of the target
		/// the solver will accept the current solution and return early.
		/// </summary>
		float solveThreshold;
	public:
		/// <summary>
		/// Create a CCD solver with default iteration and threshold values
		/// </summary>
		CCDSolver();

		// Boiler plate getter and setters

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
		/// Get a bone in the solver's IK chain in local bone space
		/// </summary>
		/// <param name="index">the index of the bone to retrieve</param>
		/// <returns>the bone stored at the given index</returns>
		transforms::srt& operator[](unsigned int index);
		
		/// <summary>
		/// Get a bone in the solver's IK chain in model space.
		/// </summary>
		/// <param name="index">the index of the bone to retrieve</param>
		/// <returns>the bone stored at the given index in model space</returns>
		transforms::srt GetBoneAt(unsigned int index);
		
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
		
		// the solver

		/// <summary>
		/// Run the IK solver to rotate the IK chain's bones so that the end effector reaches a target.
		/// Solving the chain will modify the solver's bone chain in-place.
		/// </summary>
		/// <param name="target">The location that the IK solver should try to move the end effector to, in model space</param>
		/// <returns>True if the end effector was able to reach the target</returns>
		bool SolveChain(const transforms::srt& target);
	};

}
