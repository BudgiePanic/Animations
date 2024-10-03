#pragma once

#include <vector>
#include "../Application.h"
#include "../animation/Pose.h"
#include "../animation/Clip.h"
#include "../render/Attribute.h"
#include "../render/Shader.h"
#include "../Vector3.h"
#include "../Mat4f.h"

namespace demos {

	class SimpleAnimationPlayer : public Application {
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	protected:
		/// <summary>
		/// The skeleton drawer sends commands to the GPU.
		/// </summary>
		class SkeletonDrawer {
		protected:
			std::vector<f3> points;
			render::Attribute<f3>* attributes;
			render::Shader* shader;
		private:
			SkeletonDrawer(const SkeletonDrawer&);
			SkeletonDrawer& operator=(const SkeletonDrawer&);
		public:
			SkeletonDrawer();
			SkeletonDrawer(unsigned int size);
			~SkeletonDrawer();
			enum class LineDrawMode {
				Lines, Loop, Strip, Points
			};
			unsigned int Size();
			void Resize(unsigned int size);
			f3& operator[](unsigned int index);
			/// <summary>
			/// Add a point to the point vector.
			/// </summary>
			/// <param name="vector"></param>
			void Add(const f3& vector);
			/// <summary>
			/// Extract bone positions from a pose and store the points in the points vector.
			/// </summary>
			void PointsFromPose();
			/// <summary>
			/// Send the points attribute buffer data to the GPU
			/// </summary>
			void UpdateBuffers();
			/// <summary>
			/// 
			/// </summary>
			/// <param name="mode">GPU draw mode</param>
			/// <param name="lineColor">The color of the line</param>
			/// <param name="worldCameraProjection">A matrix that converts a skeleton from local space to: world space, camera space, and then NDC</param>
			void Draw(LineDrawMode mode, const f3& lineColor, const mat4f worldCameraProjection);
		};
		anim::Pose restPose;
		/// <summary>
		/// The current model pose
		/// </summary>
		anim::Pose pose;
		/// <summary>
		/// An animation clip contains animation tracks for each bone. A track describes how the bone changes over time during the animation. 
		/// </summary>
		std::vector<anim::Clip> clips;
		unsigned int activeClip;
		float timePlayback;
		SkeletonDrawer* restPoseDrawer;
		SkeletonDrawer* poseDrawer;
	};
}
