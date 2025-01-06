#include "AnimationBlending.h"
#include <iostream>
#include "../io/gltfLoader.h"
#include "../render/Uniform.h"
#include "../animation/Blending.h"

namespace demos {
	void AnimationBlending::Initialize() {
		std::cout << "Looking for \'\\resource\\assets\\Woman.gltf\' in working directory.\n";
		cgltf_data* data = io::LoadGLTFFile("./resource/assets/Woman.gltf");
		if (data == nullptr) {
			std::cout << "couldn't load gltf data\n";
		}
		this->armature = io::MakeArmature(data);
		this->clips = io::LoadClips(data);
		this->meshes = io::LoadMeshes(data);
		io::FreeGLTFData(data);

		this->pose = this->armature.GetRestPose();
		this->pose.ToMatrixPalette(this->bonesAsMatrices);
		this->skinningMatrices = this->armature.GetInverseBindPose();

		this->shader = new render::Shader(
			"./resource/shaders/skinned_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		this->texture = new render::Texture("./resource/assets/Woman.png");

		this->left.pose = this->pose;
		this->right.pose = this->pose;

		this->blendTime = 0.0f;
		this->blendInvert = false;

		for (unsigned int i = 0; i < (unsigned int)this->clips.size(); i++) {
			if (this->clips[i].GetClipName() == "Walking") {
				this->left.clip = i;
				this->left.time = this->clips[i].GetStartTime();
			}
			else if (this->clips[i].GetClipName() == "Running") {
				this->right.clip = i;
				this->right.time = this->clips[i].GetStartTime();
			}
		}
	}

	void AnimationBlending::ShutDown() {
		this->clips.clear();
		this->meshes.clear();
		delete this->shader;
		delete this->texture;
	}

	void AnimationBlending::Update(float deltaTime) {
		this->left.time = this->clips[this->left.clip].Sample(this->left.pose, this->left.time + deltaTime);
		this->right.time = this->clips[this->right.clip].Sample(this->right.pose, this->right.time + deltaTime);

		float time = this->blendTime;
		time = time > 1.0f ? 1.0f : (time < 0.0f ? 0.0f : time);
		time = blendInvert ? 1.0f - time : time;

		anim::Blend(this->pose, this->left.pose, this->right.pose, time, -1);

		this->pose.ToMatrixPalette(this->bonesAsMatrices);

		this->blendTime += deltaTime;

		if (blendTime >= 2.0f) {
			blendTime = 0.0f;
			this->blendInvert = !this->blendInvert;
			this->pose = this->armature.GetRestPose();
		}
	}

	void AnimationBlending::Render(float aspectRatio) {
		mat4f projection = perspective(60.0f, aspectRatio, 0.01f, 1000.0f);
		mat4f view = lookAt(f3(0, 3, 5), f3(0, 3, 0), f3(0, 1, 0));
		mat4f model; // just the identity matrix today, we're operating in local model space only

		this->shader->Bind();
		bool wasUniformFound;

		this->texture->Set(this->shader->GetUniform("tex0", wasUniformFound), 0);

		render::Uniform<mat4f>::Set(this->shader->GetUniform("model_transform", wasUniformFound), model);
		render::Uniform<mat4f>::Set(this->shader->GetUniform("view_transform", wasUniformFound), view);
		render::Uniform<mat4f>::Set(this->shader->GetUniform("projection", wasUniformFound), projection);
		render::Uniform<mat4f>::Set(this->shader->GetUniform("inverseBindPose", wasUniformFound), this->skinningMatrices);
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
