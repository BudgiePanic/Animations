#pragma once

#include "../Application.h"
#include "../animation/Armature.h"
#include "../animation/Pose.h"
#include "../animation/Clip.h"

#include "../render/Mesh.h"
#include "../render/Shader.h"
#include "../render/Texture.h"


namespace demos {

	class AnimationAdding : public Application {
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	protected:
		render::Texture* texture;
		render::Shader* shader;
		std::vector<render::Mesh> meshes;
		std::vector<anim::Clip> clips;
		anim::Armature armature;
		std::vector<mat4f> bonesAsMatrices;

		unsigned int clip;
		unsigned int addingClip;

		float time;
		// The percentage of animation to add 
		float addPercentage;
		// Whether the add percentage should go up or down while the application is running
		bool increaseAdd;

		anim::Pose currentPose;
		anim::Pose addPose;
		anim::Pose addBase;
	};

}
