#include "AnimatedModel.h"
#include <iostream>
#include "../io/gltfLoader.h"
#include "../render/Uniform.h"

namespace demos {

	void AnimatedModel::Initialize() {
		std::cout << "Looking for \'\\resource\\assets\\Woman.gltf\' in working directory.\n";
		cgltf_data* data = io::LoadGLTFFile("./resource/assets/Woman.gltf");
		if (data == nullptr) {
			std::cout << "couldn't load gltf data\n";
		}
		this->cpuMeshes = io::LoadMeshes(data);
		this->skeleton = io::MakeArmature(data);
		this->clips = io::LoadClips(data);
		io::FreeGLTFData(data);

		this->gpuMeshes = this->cpuMeshes;
		// upload gpu mesh data to the GPU because the skinning will be done on the GPU
		for (unsigned int i = 0, numbMeshes = (unsigned int)this->gpuMeshes.size(); i < numbMeshes; i++) {
			this->gpuMeshes[i].SyncOpenGL();
		}

		this->cpuShader = new render::Shader(
			"./resource/shaders/static_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		this->gpuShader = new render::Shader(
			"./resource/shaders/skinned_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		this->texture = new render::Texture("./resources/assets/Woman.png");

		unsigned int numbClips = this->clips.size();
		unsigned int clipToPlay = 0;
		for (unsigned int i = 0; i < numbClips; i++) {
			if (this->clips[i].GetClipName() == "Walking") {
				clipToPlay = i;
				break;
			}
		}

		this->cpuAnimation.currentPose = this->skeleton.GetRestPose();
		this->cpuAnimation.bonesAsMatrices.resize(this->skeleton.GetRestPose().Size());
		this->cpuAnimation.transform.position = f3(2, 0, 0);
		this->cpuAnimation.currentClip = clipToPlay;

		this->gpuAnimation.currentPose = this->skeleton.GetRestPose();
		this->gpuAnimation.bonesAsMatrices.resize(this->skeleton.GetRestPose().Size());
		this->gpuAnimation.transform.position = f3(-2,0,0);
		this->gpuAnimation.currentClip = clipToPlay;

	}

	void AnimatedModel::ShutDown() {
		delete this->cpuShader;
		delete this->gpuShader;
		delete this->texture;
		// the vectors will call delete on their contents
		this->clips.clear();
		this->cpuMeshes.clear();
		this->gpuMeshes.clear();
	}

	void AnimatedModel::Update(float deltaTime) {
		// update the animated pose by sampling the animations
		AnimationData& cpuSide = this->cpuAnimation;
		cpuSide.playbackTime = 
			this->clips[cpuSide.currentClip].Sample(cpuSide.currentPose, cpuSide.playbackTime + deltaTime);

		AnimationData& gpuSide = this->gpuAnimation;
		gpuSide.playbackTime =
			this->clips[gpuSide.currentClip].Sample(gpuSide.currentPose, gpuSide.playbackTime + deltaTime);
		// =====================

		// perform CPU skinning
		for (unsigned int i = 0, numbMeshes = (unsigned int)this->cpuMeshes.size(); i < numbMeshes; i++) {
			this->cpuMeshes[i].Skin(this->skeleton, this->cpuAnimation.currentPose); // mutates the cpu meshes internal data structures
		}
		// write the T posed GPU meshes to vector as preperation for rendering/GPU skinning
		this->gpuAnimation.currentPose.ToMatrixPalette(this->gpuAnimation.bonesAsMatrices);
	}

	void AnimatedModel::Render(float aspectRatio) {
		mat4f projection = perspective(60.0f, aspectRatio, 0.01f, 1000.0f); // converts camera space to screen space
		mat4f view = lookAt(f3(0, 5, 7), f3(0, 3, 0), f3(0, 1, 0));			// converts world space to camera space
		mat4f model;														// converts model space to world space
		// render the CPU skinned mesh
		this->cpuShader->Bind();
		model = transforms::toMatrix(this->cpuAnimation.transform);
		// upload uniforms to GPU
		bool wasUniformFound;
		this->texture->Set(this->cpuShader->GetUniform("tex0", wasUniformFound), 0);
		render::Uniform<mat4f>::Set(this->cpuShader->GetUniform("model_transform", wasUniformFound), model);
		render::Uniform<mat4f>::Set(this->cpuShader->GetUniform("view_transform", wasUniformFound), view);
		render::Uniform<mat4f>::Set(this->cpuShader->GetUniform("projection", wasUniformFound), projection);
		render::Uniform<f3>::Set(this->cpuShader->GetUniform("sky_light_direction", wasUniformFound), f3(1, 1, 1));
		for (unsigned int i = 0, numbMeshes = (unsigned int)this->cpuMeshes.size(); i < numbMeshes; i++) {
			// upload per vertex data to GPU
			constexpr int noBone = -1;
			int position = this->cpuShader->GetAttribute("position", wasUniformFound);
			int normal = this->cpuShader->GetAttribute("normal", wasUniformFound);
			int texCoord = this->cpuShader->GetAttribute("textureCoordinate", wasUniformFound);
			this->cpuMeshes[i].Bind(position, normal, texCoord, noBone, noBone);
			this->cpuMeshes[i].Draw();
			this->cpuMeshes[i].UnBind(position, normal, texCoord, noBone, noBone);
		}

		this->texture->Unset(0);
		this->cpuShader->Unbind();

		// render the GPU skinned mesh (it should look the same as the CPU skinned mesh)
		this->gpuShader->Bind();
		model = transforms::toMatrix(this->gpuAnimation.transform);
		// upload uniforms to GPU
		this->texture->Set(this->gpuShader->GetUniform("tex0", wasUniformFound), 0);
		render::Uniform<mat4f>::Set(this->gpuShader->GetUniform("model_transform", wasUniformFound), model);
		render::Uniform<mat4f>::Set(this->gpuShader->GetUniform("view_transform", wasUniformFound), view);
		render::Uniform<mat4f>::Set(this->gpuShader->GetUniform("projection", wasUniformFound), projection);
		render::Uniform<f3>::Set(this->gpuShader->GetUniform("sky_light_direction", wasUniformFound), f3(1, 1, 1));
		render::Uniform<mat4f>::Set(this->gpuShader->GetUniform("posedBones", wasUniformFound), this->gpuAnimation.bonesAsMatrices);
		render::Uniform<mat4f>::Set(this->gpuShader->GetUniform("inverseBindPose", wasUniformFound), this->skeleton.GetInverseBindPose());

		for (unsigned int i = 0, numbMeshes = (unsigned int)this->gpuMeshes.size(); i < numbMeshes; i++) {
			// upload per vertex data to GPU
			int position = this->gpuShader->GetAttribute("position", wasUniformFound);
			int normal = this->gpuShader->GetAttribute("normal", wasUniformFound);
			int texCoord = this->gpuShader->GetAttribute("textureCoordinate", wasUniformFound);
			int boneWeights = this->gpuShader->GetAttribute("boneWeights", wasUniformFound);
			int boneIndices = this->gpuShader->GetAttribute("boneIndices", wasUniformFound);
			this->gpuMeshes[i].Bind(position, normal, texCoord, boneIndices, boneWeights);
			this->gpuMeshes[i].Draw();
			this->gpuMeshes[i].UnBind(position, normal, texCoord, boneIndices, boneWeights);
		}

		this->texture->Unset(0);
		this->gpuShader->Unbind();
	}

}
