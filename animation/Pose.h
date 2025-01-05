#pragma once
#include <vector>
#include "../transforms/srt.h"
#include "../Mat4f.h"

namespace anim {
	/// <summary>
	/// A pose is a collection of bones plus bone parent hierarchy
	/// </summary>
	class Pose {
	protected:
		std::vector<transforms::srt> bones;
		/// <summary>
		/// Stores the index of each bone's parent transform in the bones array.
		/// Negative index means the bone has no parent.
		/// </summary>
		std::vector<int> boneParents;
	public:
		Pose();
		Pose(unsigned int numbBones);
		/// <summary>
		/// Copy constructor.
		/// </summary>
		/// <param name="pose"></param>
		Pose(const Pose& pose);
		/// <summary>
		/// Deep copy on assignment
		/// </summary>
		/// <param name="pose"></param>
		/// <returns></returns>
		Pose& operator=(const Pose& pose);
		void Resize(unsigned int numbBones);
		unsigned int Size();
		/// <summary>
		/// Find the index of a bone's parent
		/// </summary>
		/// <param name="boneIndex">The index of the bone whose parent we are trying to find</param>
		/// <returns>The index of the bone's parent. Result is negative if bone has no parent.</returns>
		int ParentIndexOf(unsigned int boneIndex);
		void SetParentIndex(unsigned int boneIndex, unsigned int parentIndex);
		transforms::srt GetLocalTransform(unsigned int boneIndex);
		void SetLocalTransform(unsigned int boneIndex, const transforms::srt& localTransform);
		transforms::srt GetWorldTransform(unsigned int boneIndex);
		transforms::srt operator[](unsigned int boneIndex);
		/// <summary>
		/// Converts each bone's world space SRT into a matrix 4 and writes matrix into the output array.
		/// Used for transferring data to the GPU.
		/// </summary>
		/// <param name="outputArray"></param>
		void ToMatrixPalette(std::vector<mat4f>& outputArray);
		bool operator==(const Pose& other);
		bool operator!=(const Pose& other);
	};

	/// <summary>
	/// Checks if a bone is a child of another bone in a pose bone hierarchy
	/// </summary>
	/// <param name="pose"></param>
	/// <param name="parentBone"></param>
	/// <param name="boneToCheck"></param>
	/// <returns>true if boneToCheck is a child of parentBone in the pose bone hierarchy</returns>
	bool IsBoneChildOf(Pose& pose, unsigned int parentBone, unsigned int boneToCheck);

	/// <summary>
	/// Combine two poses together. Assumes the bone hierarchy in each pose is the same.
	/// </summary>
	/// <param name="poseOut">Where the resulting blended pose should be written to</param>
	/// <param name="a">first pose to combine</param>
	/// <param name="b">second pose to combine</param>
	/// <param name="t">percentage amount of pose 'b' to include. i.e. 0 implies all pose 'a', no pose 'b'</param>
	/// <param name="rootBone">The starting bone of pose B which is blended into pose 'a'. i.e. left arm to only apply blend to an arm</param>
	void Blend(Pose& poseOut, Pose& a, Pose& b, float t, int rootBone);
}
