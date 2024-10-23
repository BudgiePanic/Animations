#pragma once

#include <vector>
#include "../Application.h"
#include "../render/Texture.h"
#include "../render/Shader.h"
#include "../render/Mesh.h"
#include "../animation/Armature.h"
#include "../animation/Clip.h"
#include "../animation/Pose.h"
#include "../Mat4f.h"
#include "../transforms/srt.h"

namespace demos {

	class AnimatedModel : public Application {
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	protected:
		/// <summary>
		/// All the data needed for CPU skinning or GPU skinning while playing back a single animation clip
		/// </summary>
		struct AnimationData {
			anim::Pose currentPose; // The animation keyframes puppet the skeletons bones into a specific pose, stored as SRTs
			std::vector<mat4f> bonesAsMatrices; // container to store each bone of the pose as matrices 
			float playbackTime; // the time that the animation clip should be sampled from
			unsigned int currentClip; // The index into the clips vector of the currently playing animation
			transforms::srt transform; // model to world space transform

			AnimationData() : currentClip(0), playbackTime(0.0f) {}
		};
	protected:
		render::Texture* texture;
		render::Shader* cpuShader;
		render::Shader* gpuShader;
		std::vector<render::Mesh> cpuMeshes; // Meshes that are skinned on the CPU and then statically rendered on the GPU
		std::vector<render::Mesh> gpuMeshes; // Meshes that are skinned on the GPU
		anim::Armature skeleton; // shared model information used by all meshes. (multiple meshes can use the same skeleton/animations)
		std::vector<anim::Clip> clips;
		AnimationData cpuAnimation; // an animation being played that is being skinned on the CPU
		AnimationData gpuAnimation;
	};
}
