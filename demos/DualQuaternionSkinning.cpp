#include "DualQuaternionSkinning.h"
#include <iostream>
#include "../io/gltfLoader.h"
#include "../render/Shader.h"
#include "../render/Uniform.h"

namespace demos {

	void demos::DualQuaternionSkinning::Initialize() {
		// dq => dual quaternion
		std::cout << "Looking for \'\\resource\\assets\\dq.gltf\' in working directory.\n";
		// Note: loading in the Woman model, causes the skinning to break completely. 
		// Happens with the book author's code too.
		// Wonder where the issue is? I strongly suspect it was caused by the loss of scale data in the DQ skinning
		cgltf_data* data = io::LoadGLTFFile("./resource/assets/dq.gltf");
		if (data == nullptr) {
			std::cout << "couldn't load gltf data\n";
		}
		this->meshes = io::LoadMeshes(data);
		this->armature = io::MakeArmature(data);
		this->clips = io::LoadClips(data);
		io::FreeGLTFData(data);


		std::cout << "Looking for \'\\resource\\shaders\\skinned_dual_quat_vert.glsl\' in working directory.\n";
		std::cout << "Looking for \'\\resource\\shaders\\frag_lit.glsl\' in working directory.\n";
		this->dualQShader = new render::Shader(
			"./resource/shaders/skinned_dual_quat_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		std::cout << "Looking for \'\\resource\\shaders\\skinned_vert.glsl\' in working directory.\n";
		this->linearBlendSkinShader = new render::Shader(
			"./resource/shaders/skinned_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		std::cout << "Looking for \'\\resource\\assets\\dq.png\' in working directory.\n";
		this->texture = new render::Texture("./resource/assets/dq.png");


		this->pose = armature.GetRestPose();
		
		this->pose.ToDualQuaternionPalette(this->bonesAsDualQuat);
		this->armature.GetInverseBindPose(this->invBindPoseAsDualQuat);

		this->pose.ToMatrixPalette(this->bonesAsMatrices);
		this->invBindPoseAsMatrices = this->armature.GetInverseBindPose();

		this->clipIndex = 0;
		this->playbackTime = 0.0f;
	}

	void DualQuaternionSkinning::ShutDown() {
		delete this->linearBlendSkinShader;
		delete this->dualQShader;
		delete this->texture;
		this->meshes.clear();
		this->clips.clear();
	}

	void DualQuaternionSkinning::Update(float deltaTime) {
		this->playbackTime = this->clips[this->clipIndex].Sample(this->pose, this->playbackTime + deltaTime);
		this->pose.ToDualQuaternionPalette(this->bonesAsDualQuat);
		this->pose.ToMatrixPalette(this->bonesAsMatrices);
	}

	void DualQuaternionSkinning::Render(float aspectRatio) {
		constexpr float fovDegrees = 60.0f;
		transforms::srt translation(f3(2,2,0), rotation::quaternion(), f3(1,1,1));
		mat4f model = transforms::toMatrix(translation);
		mat4f view = lookAt(f3(0,3,14), f3(0,0,0), f3(0,1,0));
		mat4f projection = perspective(fovDegrees, aspectRatio, 0.01, 1000.0f);

		this->dualQShader->Bind();
		bool wasUniformFound;
		render::Uniform<mat4f>::Set(dualQShader->GetUniform("model", wasUniformFound), model);
		render::Uniform<mat4f>::Set(dualQShader->GetUniform("view", wasUniformFound), view);
		render::Uniform<mat4f>::Set(dualQShader->GetUniform("projection", wasUniformFound), projection);
		render::Uniform<f3>::Set(dualQShader->GetUniform("sky_light_direction", wasUniformFound), f3(1,1,1));
		render::Uniform<transforms::DualQuaternion>::Set(dualQShader->GetUniform("posedBones", wasUniformFound), this->bonesAsDualQuat);
		render::Uniform<transforms::DualQuaternion>::Set(dualQShader->GetUniform("inverseBindPose", wasUniformFound), this->invBindPoseAsDualQuat);
		
		this->texture->Set(this->dualQShader->GetUniform("tex0", wasUniformFound), 0);

		for (unsigned int i = 0, size = (unsigned int)this->meshes.size(); i < size; i++) {
			unsigned int position = this->dualQShader->GetAttribute("position", wasUniformFound);
			unsigned int normal = this->dualQShader->GetAttribute("normal", wasUniformFound);
			unsigned int textureCoordinate = this->dualQShader->GetAttribute("textureCoordinate", wasUniformFound);
			unsigned int boneWeights = this->dualQShader->GetAttribute("boneWeights", wasUniformFound);
			unsigned int boneIndices = this->dualQShader->GetAttribute("boneIndices", wasUniformFound);
			this->meshes[i].Bind(position, normal, textureCoordinate, boneIndices, boneWeights);
			this->meshes[i].Draw();
			this->meshes[i].UnBind(position, normal, textureCoordinate, boneIndices, boneWeights);
		}
		this->texture->Unset(0);
		this->dualQShader->Unbind();
		// =================================

		this->linearBlendSkinShader->Bind();

		translation.position.x = -2;
		translation.position.y = -2;
		model = transforms::toMatrix(translation);

		render::Uniform<mat4f>::Set(this->linearBlendSkinShader->GetUniform("model_transform", wasUniformFound), model);
		render::Uniform<mat4f>::Set(this->linearBlendSkinShader->GetUniform("view_transform", wasUniformFound), view);
		render::Uniform<mat4f>::Set(this->linearBlendSkinShader->GetUniform("projection", wasUniformFound), projection);
		render::Uniform<f3>::Set(this->linearBlendSkinShader->GetUniform("sky_light_direction", wasUniformFound), f3(1, 1, 1));
		render::Uniform<mat4f>::Set(this->linearBlendSkinShader->GetUniform("posedBones", wasUniformFound), this->bonesAsMatrices);
		render::Uniform<mat4f>::Set(this->linearBlendSkinShader->GetUniform("inverseBindPose", wasUniformFound), this->invBindPoseAsMatrices);

		this->texture->Set(this->linearBlendSkinShader->GetUniform("tex0", wasUniformFound), 0);

		for (unsigned int i = 0, size = (unsigned int)this->meshes.size(); i < size; i++) {
			unsigned int position = this->linearBlendSkinShader->GetAttribute("position", wasUniformFound);
			unsigned int normal = this->linearBlendSkinShader->GetAttribute("normal", wasUniformFound);
			unsigned int textureCoordinate = this->linearBlendSkinShader->GetAttribute("textureCoordinate", wasUniformFound);
			unsigned int boneWeights = this->linearBlendSkinShader->GetAttribute("boneWeights", wasUniformFound);
			unsigned int boneIndices = this->linearBlendSkinShader->GetAttribute("boneIndices", wasUniformFound);
			this->meshes[i].Bind(position, normal, textureCoordinate, boneIndices, boneWeights);
			this->meshes[i].Draw();
			this->meshes[i].UnBind(position, normal, textureCoordinate, boneIndices, boneWeights);
		}
		this->texture->Unset(0);
		this->linearBlendSkinShader->Unbind();
	}
}
