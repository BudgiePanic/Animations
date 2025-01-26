#pragma once
#include <vector>
#include <string>
#include "Pose.h"
#include "../Mat4f.h"

namespace anim {

	/// <summary>
	/// An armature stores all the shared data between instances of animated meshes.
	/// They store the rest pose, bind pose, inverse bind pose, and bone names.
	/// The armature may also be known as a skeleton or a rig.
	/// </summary>
	class Armature {
	protected:
		Pose restPose;
		/// <summary>
		/// The bind pose matches to the model space vertices of the mesh when T-posed.
		/// </summary>
		Pose bindPose;
		/// <summary>
		/// Converts model space vertices to be in a space relative to a bone.
		/// </summary>
		std::vector<mat4f> inverseBindPose;
		std::vector<std::string> boneNames;
	protected:
		/// <summary>
		/// Recalculates the inverse bind pose. 
		/// Call after updating the bindPose member variable.
		/// </summary>
		void RecalculateInverseBindPose();
	public:
		Armature();
		Armature(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);
		void Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);
		Pose& GetBindPose();
		Pose& GetRestPose();
		std::vector<mat4f>& GetInverseBindPose();
		void GetInverseBindPose(std::vector<transforms::DualQuaternion>& outputArray);
		std::vector<std::string>& GetBoneNames();
		std::string& GetBoneName(unsigned int index);
	};
}
