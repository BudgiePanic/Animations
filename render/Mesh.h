#pragma once
#include <vector>
#include "../Vector2.h"
#include "../Vector3.h"
#include "../Vector4.h"
#include "../Mat4f.h"
#include "Attribute.h"
#include "IndexBuffer.h"
#include "../animation/Armature.h"
#include "../animation/Pose.h"

namespace render {

	/// <summary>
	/// A mesh stores the vertices and their attributes of a 3D model.
	/// </summary>
	class Mesh {
	protected:
		// CPU data...
		std::vector<f3> positions;
		std::vector<f3> normals;
		std::vector<f2> textureCoords;
		std::vector<i4> boneIndices;
		std::vector<f4> boneWeights;
		// Vertex indices are optional.
		std::vector<unsigned int> vertexIndices;
	protected:
		// ...is synchronised with GPU data.
		Attribute<f3>* positionAttribute;
		Attribute<f3>* normalAttribute;
		Attribute<f2>* textureCoordAttribute;
		Attribute<i4>* boneIndexAttribute;
		Attribute<f4>* boneWeightAttribute;
		// Vertex indices are optional.
		IndexBuffer* vertexIndexBuffer;
	protected:
		// Information needed to perform CPU side skinning
		std::vector<f3> skinnedPositions;
		std::vector<f3> skinnedNormals;
		/// <summary>
		/// Each bone in world space converted into mat4f, from an animated pose.
		/// </summary>
		std::vector<mat4f> posedBones;
	public:
		/// <summary>
		/// Default Constructor
		/// </summary>
		Mesh();
		/// <summary>
		/// Copy Constructor
		/// </summary>
		/// <param name="other"></param>
		Mesh(const Mesh& other);
		/// <summary>
		/// Copy on assignment
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Mesh& operator=(const Mesh& other);
		/// <summary>
		/// Destructor
		/// </summary>
		~Mesh();
	public:
		std::vector<f3>& GetPositions();
		std::vector<f3>& GetNormals();
		std::vector<f2>& GetTextureCoords();
		std::vector<i4>& GetBoneIndices();
		std::vector<f4>& GetBoneWeights();
		std::vector<unsigned int>& GetVertexIndices();
	public:
		/// <summary>
		/// Performs CPU side mesh skinning.
		/// </summary>
		/// <param name="skeleton"></param>
		/// <param name="animatedPose"></param>
		void Skin(anim::Armature& skeleton, anim::Pose& animatedPose);
		/// <summary>
		/// Synchronises the data on the GPU with the data stored in this mesh.
		/// Should be called after skinning is completed.
		/// </summary>
		void SyncOpenGL();
		/// <summary>
		/// TODO document what this function does...
		/// </summary>
		/// <param name="positionIdx">TODO</param>
		/// <param name="normalIdx">TODO</param>
		/// <param name="textureCoordIdx">TODO</param>
		/// <param name="boneIdx">TODO</param>
		/// <param name="boneWeightIdx">TODO</param>
		void Bind(int positionIdx, int normalIdx, int textureCoordIdx, int boneIdx, int boneWeightIdx);
		/// <summary>
		/// TODO document what this function does
		/// </summary>
		/// <param name="positionIdx"></param>
		/// <param name="normalIdx"></param>
		/// <param name="textureCoordIdx"></param>
		/// <param name="boneIdx"></param>
		/// <param name="boneWeightIdx"></param>
		void UnBind(int positionIdx, int normalIdx, int textureCoordIdx, int boneIdx, int boneWeightIdx);
		/// <summary>
		/// TODO document what this function does
		/// </summary>
		void Draw();
		/// <summary>
		/// TODO document what this function does
		/// </summary>
		/// <param name="numbInstances"></param>
		void DrawInstanced(unsigned int numbInstances);
	};
}
