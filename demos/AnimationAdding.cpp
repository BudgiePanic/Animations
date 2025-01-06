#include "AnimationAdding.h"
#include <iostream>
#include "../io/gltfLoader.h"
#include "../render/Uniform.h"
#include "../animation/Blending.h"

namespace demos {

	void AnimationAdding::Initialize() {
		// this is becoming boilerplate at this point
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
		// end of boiler plate

		this->increaseAdd = true;
		this->addPercentage = 0.0f;
		this->time = 0.0f;
		this->clip = 0;
		this->addingClip = 0;

		for (unsigned int i = 0; i < (unsigned int)this->clips.size(); i++) {
			if (this->clips[i].GetClipName() == "Walking") {
				this->clip = i;
			}
			else if (this->clips[i].GetClipName() == "Lean_Left") {
				this->addingClip = i;
			}
		}

		this->addBase = anim::MakePoseForAdding(this->armature, this->clips[this->addingClip]);
		this->clips[this->addingClip].SetClipLooping(false);

		this->currentPose = armature.GetRestPose();
		this->addPose = armature.GetRestPose();
	}

	void AnimationAdding::ShutDown() {
		this->clips.clear();
		this->meshes.clear();
		delete this->shader;
		delete this->texture;
	}

	const float playTimeMax = 5.0f;
	float playTime = 0.0f;

	void AnimationAdding::Update(float deltaTime) {
		this->addPercentage += (this->increaseAdd ? 1.0f : -1.0f) * deltaTime;

		if (this->addPercentage > 1.0f) {
			this->addPercentage = 1.0f;
			this->increaseAdd = !this->increaseAdd;
		}
		if (this->addPercentage < 0.0f) {
			this->addPercentage = 0.0f;
			this->increaseAdd = !this->increaseAdd;
		}

		playTime += deltaTime;
		if (playTime > playTimeMax) {
			playTime = 0.0f;
			this->clip += 1;
			if (this->clip == this->addingClip) { this->clip += 1; }
			if (this->clip >= this->clips.size()) { this->clip = 0; }
		}

		this->time = this->clips[this->clip].Sample(this->currentPose, this->time + deltaTime);

		anim::Clip& addClip = this->clips[this->addingClip];
		float addTime = addClip.GetStartTime() + (addClip.GetDuration() * this->addPercentage);
		addClip.Sample(this->addPose, addTime);

		anim::AddToPose(this->currentPose, this->currentPose, this->addPose, this->addBase, -1);

		this->currentPose.ToMatrixPalette(this->bonesAsMatrices);
	}

	void AnimationAdding::Render(float aspectRatio)	{
		// the render code is also starting to become boiler plate?
		// working in local model space, so the model matrix is the identity
		mat4f model;
		mat4f view = lookAt(f3(0, 3, 7), f3(0, 3, 0), f3(0, 1, 0));
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
