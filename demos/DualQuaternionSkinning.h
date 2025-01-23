#pragma once

#include <vector>

#include "../Application.h"

#include "../transforms/DualQuaternion.h"

#include "../render/Texture.h"
#include "../render/Shader.h"
#include "../render/Mesh.h"

#include "../animation/Armature.h"
#include "../animation/Clip.h"

namespace demos {

	/// <summary>
	/// Dual quaternion skinning demo demonstrates how dual quaternions can be used to skin a mesh
	/// instead of linear blended skinning via the use of matrices. 
	/// It highlights the volume preserving qualities of dual quaternion skinning when rotating a mesh.
	/// </summary>
	class DualQuaternionSkinning : public Application {
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	protected:
		render::Texture* texture;
		render::Shader* dualQShader;
		render::Shader* linearBlendSkinShader;
		std::vector<render::Mesh> meshes;
		anim::Armature armature;
		anim::Pose pose;

		std::vector<transforms::DualQuaternion> bonesAsDualQuat;
		std::vector<transforms::DualQuaternion> invBindPoseAsDualQuat;

		std::vector<mat4f> bonesAsMatrices;
		std::vector<mat4f> invBindPoseAsMatrices;

		std::vector<anim::Clip> clips;
		unsigned int clipIndex;
		float playbackTime;
	};
}
