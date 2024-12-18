#pragma once
#include <map>
#include "Armature.h"
#include "../render/Mesh.h"
#include "Clip.h"
// TODO move to more relevant folder than animation folder?
/// <summary>
/// Rearrangement contains functions that reorder the bones in various data structures.
/// </summary>
namespace anim {
	/// <summary>
	/// Rearrage the order of bones in an armature so that the bones are stored in ascending order
	/// </summary>
	/// <param name="armature">the armature to rearrange</param>
	/// <returns>a mapping that shows where the bones were reordered to e.g. the bone at index 0 was moved to index 5</returns>
	std::map<int, int> RearrangeArmature(Armature& armature);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="mesh"></param>
	/// <param name="boneRemapping"></param>
	void RearrangeMesh(render::Mesh& mesh, std::map<int, int>& boneRemapping);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="clip"></param>
	/// <param name="boneRemapping">A mapping to show where each bone should be remapped to</param>
	void RearrangeClip(Clip& clip, std::map<int, int>& boneRemapping);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="clip"></param>
	/// <param name="boneRemapping">A mapping to show where each bone should be remapped to</param>
	void RearrangeQuickClip(QuickClip& clip, std::map<int, int>& boneRemapping);
}
