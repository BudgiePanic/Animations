#include "CrossFadedAnimations.h"
#include <iostream>
#include "../io/gltfLoader.h"
#include "../render/Uniform.h"
#include "../animation/Blending.h"

namespace demos {
	
	const float timerDefault = 5.0f;

	void CrossFadedAnimations::Initialize() {
		std::cout << "Looking for \'\\resource\\assets\\Woman.gltf\' in working directory.\n";
		cgltf_data* data = io::LoadGLTFFile("./resource/assets/Woman.gltf");
		if (data == nullptr) {
			std::cout << "couldn't load gltf data\n";
		}
		this->armature = io::MakeArmature(data);
		this->clips = io::LoadClips(data);
		this->meshes = io::LoadMeshes(data);
		io::FreeGLTFData(data);

		this->shader = new render::Shader(
			"./resource/shaders/skinned_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		this->texture = new render::Texture("./resource/assets/Woman.png");

		this->controller.SetArmature(this->armature);
		this->controller.Play(&this->clips[0]);
		this->controller.Update(0.0f);
		this->controller.GetCurrentPose().ToMatrixPalette(this->bonesAsMatrices);

		this->timer = timerDefault;
		this->currentClip = 0;
	}

	void CrossFadedAnimations::ShutDown() {
		this->clips.clear();
		this->meshes.clear();
		delete this->shader;
		delete this->texture;
	}

	void CrossFadedAnimations::Update(float deltaTime) {
		this->controller.Update(deltaTime);
		this->timer -= deltaTime;

		if (this->timer < 0.0f) {
			this->timer = timerDefault;
			// pick a new clip 
			unsigned int nextClip = this->currentClip;
			while (true) {
				nextClip = rand() % this->clips.size();
				if (nextClip != this->currentClip) { break; }
			}
			this->currentClip = nextClip;
			// and fade to it
			this->controller.FadeTo(&this->clips[this->currentClip], 1.5f);
		}

		this->controller.GetCurrentPose().ToMatrixPalette(this->bonesAsMatrices);
	}

	void CrossFadedAnimations::Render(float aspectRatio) {
		// working in local model space, so the model matrix is the identity
		mat4f model;
		mat4f view = lookAt(f3(0,3,7), f3(0,3,0), f3(0, 1, 0));
		mat4f projection = perspective(60.0f, aspectRatio, 0.01f, 1000.0f);

		this->shader->Bind();
		bool wasUniformFound;

		this->texture->Set(this->shader->GetUniform("tex0", wasUniformFound), 0);

		render::Uniform<mat4f>::Set(this->shader->GetUniform("model_transform", wasUniformFound), model);
		render::Uniform<mat4f>::Set(this->shader->GetUniform("view_transform", wasUniformFound), view);
		render::Uniform<mat4f>::Set(this->shader->GetUniform("projection", wasUniformFound), projection);
		render::Uniform<mat4f>::Set(this->shader->GetUniform("inverseBindPose", wasUniformFound), this->armature.GetInverseBindPose());
		render::Uniform<mat4f>::Set(this->shader->GetUniform("posedBones", wasUniformFound), this->bonesAsMatrices);

		render::Uniform<f3>::Set(this->shader->GetUniform("sky_light_direction", wasUniformFound), f3(1, 1, 1));

		for (unsigned int i = 0; i < (unsigned int)this->meshes.size(); i++) {
			int position = this->shader->GetAttribute("position", wasUniformFound);
			int normal = this->shader->GetAttribute("normal", wasUniformFound);
			int texCoord = this->shader->GetAttribute("textureCoordinate", wasUniformFound);
			int boneWeight = this->shader->GetAttribute("boneWeights", wasUniformFound);
			int boneIndex = this->shader->GetAttribute("boneIndices", wasUniformFound);
			this->meshes[i].Bind(position, normal, texCoord, boneIndex, boneWeight);
			this->meshes[i].Draw();
			this->meshes[i].UnBind(position, normal, texCoord, boneIndex, boneWeight);
		}

		this->texture->Unset(0);
		this->shader->Unbind();
	}

}
