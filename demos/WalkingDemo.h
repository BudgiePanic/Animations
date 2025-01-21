#pragma once

#include <vector>

#include "../Application.h"
#include "InverseKinematicsDemo.h"

#include "../render/Texture.h"
#include "../render/Mesh.h"

#include "../animation/Clip.h"
#include "../animation/Pose.h"

#include "../transforms/srt.h"

namespace demos {

	/// <summary>
	/// Demonstrates using inverse kinematics to place the characters feet on an uneven surface.
	/// </summary>
	class WalkingDemo : public Application {
	protected:
		// data structures and algorithms for ray casting intersections with triangles
		struct Ray {
			f3 origin;
			f3 direction;

			/// <summary>
			/// Create a ray that points straight down, at the world origin
			/// </summary>
			Ray() : direction(f3(0,-1,0)), origin(f3(0,0,0)) {}

			/// <summary>
			/// Create a ray at a location that points straight down
			/// </summary>
			/// <param name="_origin"></param>
			Ray(const f3& _origin) : direction(f3(0, -1, 0)), origin(_origin) {}

			Ray(const f3& _origin, const f3& _direction) : direction(_direction), origin(_origin) {}
		};
		
		struct Triangle {
			/// <summary>
			/// The three vertices of the triangle
			/// </summary>
			f3 v0, v1, v2;
			f3 normal;

			Triangle() {}
			Triangle(const f3& _v0, const f3& _v1, const f3& _v2) : v0(_v0), v1(_v1), v2(_v2) {
				// same algorithm as used in PushingP
				normal = normalized(cross(v0 - v1, v2 - v0));
			}
		};
		std::vector<Triangle> TrianglesFromMesh(render::Mesh& meshy);
		std::vector<Triangle> TrianglesFromMeshes(std::vector<render::Mesh>& meshes);

		/// <summary>
		/// Perform a ray cast intersection test against a triangle.
		/// This function returns false if the intersection point is behind the ray origin
		/// </summary>
		/// <param name="ray"></param>
		/// <param name="triangle"></param>
		/// <param name="intersectPointOut">The point in space where the ray intersected with the triangle. Only valid if function returns true</param>
		/// <returns>True if the ray hit the triangle</returns>
		bool Intersect(const Ray& ray, const Triangle& triangle, f3& intersectPointOut);

		struct RenderOptions {
			bool IKPose;
			bool currentPose;
			bool depthTest;
			bool mesh;
			bool floor;
		};

		struct DemoOptions {
			bool useFootIK;
			bool useToeIk;
			float playBackSpeed;
		};

		/// <summary>
		/// Inverse Kinematic leg stores all the information needed to position a leg with inverse kinematics
		/// </summary>
		class IKLeg {
		protected:
			/// <summary>
			/// 1 dimensional track that records the height of the leg over the duration of the walk cycle
			/// </summary>
			anim::TrackScalar legHeight;
			ik::FABRIKSolver solver;
			/// <summary>
			/// legPose stores the pose after inverse kinematics has been used to solve where the foot should be placed
			/// </summary>
			anim::Pose legPose;
			// bone indices
			unsigned int hipIndex, kneeIndex, ankleIndex, toeIndex;

			InverseKinematicsDemo::LineDrawer* legVisualizer;

			float ankleGroundOffset;
		public:
			IKLeg();
			IKLeg(anim::Armature& armature, const std::string hip, const std::string knee, const std::string ankle, const std::string toe);
			IKLeg(const IKLeg& leg);
			IKLeg& operator=(const IKLeg& leg);
			~IKLeg();

			void Sovle(const transforms::srt& modelTransform, anim::Pose& pose, const f3& footPlacementTarget);

			/// <summary>
			/// Get the pose that has had its foot placement adjusted.
			/// </summary>
			/// <returns></returns>
			anim::Pose& GetPose();
			anim::TrackScalar& GetLegHeightTrack();

			void drawDebugLeg(const mat4f& viewProjection, const f3& color);

			// bone index getters
			unsigned int GetHip();
			unsigned int GetKnee();
			unsigned int GetAnkle();
			unsigned int GetToe();

			void SetAnkleGroundOffset(float offset);
		};
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	protected:
		render::Texture* floorTexture;
		std::vector<render::Mesh> floorMeshes;
		std::vector<Triangle> floorTriangles;
		render::Shader* floorShader;

		/// <summary>
		/// The predefined path that the actor walks around the floor
		/// </summary>
		anim::TrackVector actorPathTrack;
		/// <summary>
		/// The amount of timespent walking around the predefined track
		/// </summary>
		float actorTime;

		transforms::srt actorTransform;
		std::vector<render::Mesh> actorMeshes;
		anim::Pose pose;
		std::vector<mat4f> bonesAsMatrices;
		render::Texture* actorTexture;
		render::Shader* actorShader;
		anim::Armature armature;
		std::vector<anim::Clip> clips;
		unsigned int clipIndex;
		float playbackTime;
		InverseKinematicsDemo::LineDrawer* poseDrawer;

		/// <summary>
		/// 
		/// </summary>
		float groundOffset;

		RenderOptions options;
		DemoOptions demoOptions;

		IKLeg* leftLeg;
		IKLeg* rightLeg;
		float toeLength;
		/// <summary>
		/// The height of the actor's model in the last update
		/// </summary>
		float prevModelHeight;
	};
}
