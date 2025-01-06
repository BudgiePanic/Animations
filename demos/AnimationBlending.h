#pragma once

#include "../Application.h"
#include "../animation/Armature.h"
#include "../animation/Pose.h"
#include "../animation/Clip.h"
#include "../render/Mesh.h"
#include "../render/Shader.h"
#include "../render/Texture.h"

namespace demos {
	struct AnimInstance {
		unsigned int clip;
		float time;
		anim::Pose pose;
	};

	class AnimationBlending : public Application {
	protected:
		render::Shader* shader;
		render::Texture* texture;
		std::vector<render::Mesh> meshes;
		std::vector<anim::Clip> clips;
		anim::Armature armature;
		anim::Pose pose;
		std::vector<mat4f> bonesAsMatrices;
		std::vector<mat4f> skinningMatrices;
		AnimInstance left;
		AnimInstance right;
		float blendTime;
		bool blendInvert;
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	};
}
