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
		std::cout << "Looking for \'\\resource\\shaders\\frag_lit.glsl\' in working directory.\n";
		std::cout << "Looking for \'\\resource\\shaders\\static_vert.glsl\' in working directory.\n";
		this->cpuShader = new render::Shader(
			"./resource/shaders/static_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		std::cout << "Looking for \'\\resource\\shaders\\skinned_vert_simple.glsl\' in working directory.\n";
		this->gpuShader = new render::Shader(
			"./resource/shaders/skinned_vert_simple.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		std::cout << "Looking for \'\\resource\\assets\\Woman.png\' in working directory.\n";
		this->texture = new render::Texture("./resource/assets/Woman.png");

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

	float elapsedTime = 0.0f;

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
			cpuSide.currentPose.ToMatrixPalette(cpuSide.bonesAsMatrices); // overwrite bonesAsMatrix vector with current animated pose
			std::vector<mat4f>& inverseBindPose = this->skeleton.GetInverseBindPose();
			for (int k = 0; k < cpuSide.bonesAsMatrices.size(); k++) {
				cpuSide.bonesAsMatrices[k] = cpuSide.bonesAsMatrices[k] * inverseBindPose[k]; // write the skinning transform into the bone buffer instead
			}
			this->cpuMeshes[i].Skin(cpuSide.bonesAsMatrices);
			/* this->cpuMeshes[i].Skin(this->skeleton, this->cpuAnimation.currentPose); // mutates the cpu meshes internal data structures | the old skinning method */
		}
		// prepare the skinning transforms for rendering/GPU skinning
		this->gpuAnimation.currentPose.ToMatrixPalette(this->gpuAnimation.bonesAsMatrices);
		std::vector<mat4f>& inverseBindPose = this->skeleton.GetInverseBindPose();
		for (int k = 0; k < gpuSide.bonesAsMatrices.size(); k++) {
			gpuSide.bonesAsMatrices[k] = gpuSide.bonesAsMatrices[k] * inverseBindPose[k];
		}
		elapsedTime += deltaTime;
		if (elapsedTime > 5.0f) {
			elapsedTime = 0.0f;
			cpuSide.currentClip = (cpuSide.currentClip + 1) % this->clips.size();
			gpuSide.currentClip = cpuSide.currentClip;
		}
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
		render::Uniform<mat4f>::Set(this->gpuShader->GetUniform("animatedBones", wasUniformFound), this->gpuAnimation.bonesAsMatrices);

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
