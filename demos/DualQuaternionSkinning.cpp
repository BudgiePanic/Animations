#include "DualQuaternionSkinning.h"
#include <iostream>
#include "../io/gltfLoader.h"
#include "../render/Shader.h"

namespace demos {

	void demos::DualQuaternionSkinning::Initialize() {
		// dq => dual quaternion
		std::cout << "Looking for \'\\resource\\assets\\dq.gltf\' in working directory.\n";
		cgltf_data* data = io::LoadGLTFFile("./resource/assets/Woman.gltf");
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
		std::cout << "Looking for \'\\resource\\assetts\\dq.png\' in working directory.\n";
		this->texture = new render::Texture("./resource/assets/dq.png");


		this->pose = armature.GetRestPose();
		
		this->pose.ToDualQuaternionPalette(this->bonesAsDualQuat);
		this->armature.GetInverseBindPose(this->invBindPoseAsDualQuat);

		this->pose.ToMatrixPalette(this->bonesAsMatrices);
		this->invBindPoseAsMatrices = this->armature.GetInverseBindPose();

		this->clipIndex = 0;
		this->playbackTime = 0.0f;
	}

}
