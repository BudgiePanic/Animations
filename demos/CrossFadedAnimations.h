#pragma once

#include "../Application.h"
#include "../animation/Armature.h"
#include "../animation/Pose.h"
#include "../animation/Clip.h"
#include "../render/Mesh.h"
#include "../render/Shader.h"
#include "../render/Texture.h"
#include "../animation/CrossFadeTarget.h"
#include "../animation/CrossFadeController.h"

namespace demos {

	class CrossFadedAnimations : public Application {
	protected:
		render::Texture* texture;
		render::Shader* shader;
		std::vector<render::Mesh> meshes;
		std::vector<anim::Clip> clips;
		anim::Armature armature;
		std::vector<mat4f> bonesAsMatrices;
		anim::CrossFadeController controller;
		unsigned int currentClip;
		float timer;
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	};
}
