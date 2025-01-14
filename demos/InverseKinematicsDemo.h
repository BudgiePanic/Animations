#pragma once

#include "../Application.h"
#include "../transforms/srt.h"
#include "../ik/CCDSolver.h"
#include "../ik/FABRIKSolver.h"
#include "SimpleAnimationPlayer.h"
#include "../animation/TransformTrack.h"

namespace demos {

	/// <summary>
	/// Demonstrates both the FABRIK and CCD solvers by moving a bone chain to chase a moving target
	/// </summary>
	class InverseKinematicsDemo : public Application {
	protected:
		/// <summary>
		/// The location that that IK solvers are trying make the end effector reach.
		/// </summary>
		transforms::srt goal;

		/// <summary>
		/// A cyclic coordinate descent solver
		/// </summary>
		ik::CCDSolver ccdSolver;

		/// <summary>
		/// A forward and backward reaching inverse kinematic solver
		/// </summary>
		ik::FABRIKSolver fabrikSolver;

		/// <summary>
		/// Draws lines connected the bone chains of the IK solvers.
		/// Basically the same as SimpleAnimationPlayer's Skeleton but with functions to 
		/// load the bone chain information from IK solvers.
		/// </summary>
		class LineDrawer : public SimpleAnimationPlayer::SkeletonDrawer {
		public:
			void FromIKSolver(ik::CCDSolver& solver);
			void FromIKSolver(ik::FABRIKSolver& solver);
			LineDrawer(unsigned int size);
			LineDrawer();
		};

		LineDrawer* ccdDrawer;
		LineDrawer* fabrikDrawer;
		LineDrawer* goalDrawer;

		/// <summary>
		/// The goal will follow a predetermined path from an animation track
		/// </summary>
		anim::SRTtrack goalPath;

		float time;

		/// <summary>
		/// Camera parameters. pitch and yaw in radians. distance is distance from world origin. 
		/// </summary>
		float pitch, yaw, distance;

		void SetFrame(anim::TrackVector& track, int frameIndex, float frameTime, const f3& frameData);
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	};

}
