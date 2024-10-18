#include "Mesh.h"
#include "Draw.h"

namespace render {

Mesh::Mesh() {
	this->boneIndexAttribute = new Attribute<i4>();
	this->boneWeightAttribute = new Attribute<f4>();
	this->normalAttribute = new Attribute<f3>();
	this->textureCoordAttribute = new Attribute<f2>();
	this->positionAttribute = new Attribute<f3>();
	this->vertexIndexBuffer = new IndexBuffer();
}

Mesh::Mesh(const Mesh& other) {
	this->boneIndexAttribute = new Attribute<i4>();
	this->boneWeightAttribute = new Attribute<f4>();
	this->normalAttribute = new Attribute<f3>();
	this->textureCoordAttribute = new Attribute<f2>();
	this->positionAttribute = new Attribute<f3>();
	this->vertexIndexBuffer = new IndexBuffer();
	*this = other;
}

Mesh& Mesh::operator=(const Mesh& other) {
	if (this == &other) { return *this; }
	this->boneIndices = other.boneIndices;
	this->boneWeights = other.boneWeights;
	this->normals = other.normals;
	this->positions = other.positions;
	this->textureCoords = other.textureCoords;
	this->vertexIndices = other.vertexIndices;
	this->SyncOpenGL();
	return *this;
}

Mesh::~Mesh() {
	delete this->boneIndexAttribute;
	delete this->boneWeightAttribute;
	delete this->normalAttribute;
	delete this->textureCoordAttribute;
	delete this->positionAttribute;
	delete this->vertexIndexBuffer;
}

std::vector<f3>& Mesh::GetPositions() { return this->positions; }

std::vector<f3>& Mesh::GetNormals() { return this->normals; }

std::vector<f2>& Mesh::GetTextureCoords() { return this->textureCoords; }

std::vector<i4>& Mesh::GetBoneIndices() { return this->boneIndices; }

std::vector<f4>& Mesh::GetBoneWeights() { return this->boneWeights; }

std::vector<unsigned int>& Mesh::GetVertexIndices() { return this->vertexIndices; }

void Mesh::Skin(anim::Armature& skeleton, anim::Pose& animatedPose) {
	unsigned int vertCount = this->positions.size();
	if (vertCount == 0) { return; } // no 'skin' to apply
	this->skinnedPositions.resize(vertCount);
	this->skinnedNormals.resize(vertCount);
	anim::Pose& bindPose = skeleton.GetBindPose();
	// skinning every vertex
	for (int i = 0; i < vertCount; i++) {
		// bones that influence this vertex
		i4& bones = this->boneIndices[i]; 
		// skin the vertex to the first bone
		transforms::srt skinningTransform0 = transforms::combine(
			animatedPose[bones.x], 
			transforms::inverse(bindPose[bones.x])
		);
		f3 skinnedVert0 = transforms::applyPoint(skinningTransform0, this->positions[i]);
		f3 skinnedNorm0 = transforms::applyVector(skinningTransform0, this->normals[i]);
		// skin the vertex to the second bone
		transforms::srt skinningTransform1 = transforms::combine(
			animatedPose[bones.y],
			transforms::inverse(bindPose[bones.y])
		);
		f3 skinnedVert1 = transforms::applyPoint(skinningTransform1, this->positions[i]);
		f3 skinnedNorm1 = transforms::applyVector(skinningTransform1, this->normals[i]);
		// skin the vertex to the third bone
		transforms::srt skinningTransform2 = transforms::combine(
			animatedPose[bones.z],
			transforms::inverse(bindPose[bones.z])
		);
		f3 skinnedVert2 = transforms::applyPoint(skinningTransform2, this->positions[i]);
		f3 skinnedNorm2 = transforms::applyVector(skinningTransform2, this->normals[i]);
		// skin the vertex to the fourth bone
		transforms::srt skinningTransform3 = transforms::combine(
			animatedPose[bones.w],
			transforms::inverse(bindPose[bones.w])
		);
		f3 skinnedVert3 = transforms::applyPoint(skinningTransform3, this->positions[i]);
		f3 skinnedNorm3 = transforms::applyVector(skinningTransform3, this->normals[i]);
		// combine skinned verts and normals into a final skinned vertex and normal
		// how much each bone affects this vertex (if a bone has no influence, its weight will be zero)
		f4& weights = this->boneWeights[i]; 
		this->skinnedPositions[i] = 
			skinnedVert0 * weights.x + 
			skinnedVert1 * weights.y +
			skinnedVert2 * weights.z +
			skinnedVert3 * weights.w;
		this->skinnedNormals[i] = 
			skinnedNorm0 * weights.x +
			skinnedNorm1 * weights.y +
			skinnedNorm2 * weights.z +
			skinnedNorm3 * weights.w;
		// tell the GPU about the work we've done
		this->positionAttribute->Set(this->skinnedPositions);
		this->normalAttribute->Set(this->skinnedNormals);
	}
}

void Mesh::SyncOpenGL() {
	if (this->boneIndices.size() > 0) {
		this->boneIndexAttribute->Set(this->boneIndices);
	}
	if (this->boneWeights.size() > 0) {
		this->boneWeightAttribute->Set(this->boneWeights);
	}
	if (this->normals.size() > 0) {
		this->normalAttribute->Set(this->normals);
	}
	if (this->positions.size() > 0) {
		this->positionAttribute->Set(this->positions);
	}
	if (this->textureCoords.size() > 0) {
		this->textureCoordAttribute->Set(this->textureCoords);
	}
	if (this->vertexIndices.size() > 0) {
		this->vertexIndexBuffer->Set(this->vertexIndices);
	}
}

void Mesh::Bind(int positionIdx, int normalIdx, int textureCoordIdx, int boneIdx, int boneWeightIdx) {
	if (boneIdx >= 0) { this->boneIndexAttribute->BindTo(boneIdx); }
	if (boneWeightIdx >= 0) { this->boneWeightAttribute->BindTo(boneWeightIdx); }
	if (normalIdx >= 0) { this->normalAttribute->BindTo(normalIdx); }
	if (positionIdx >= 0) { this->positionAttribute->BindTo(positionIdx); }
	if (textureCoordIdx >= 0) { this->textureCoordAttribute->BindTo(textureCoordIdx); }
}

void Mesh::UnBind(int positionIdx, int normalIdx, int textureCoordIdx, int boneIdx, int boneWeightIdx) {
	if (boneIdx >= 0) { this->boneIndexAttribute->UnBindFrom(boneIdx); }
	if (boneWeightIdx >= 0) { this->boneWeightAttribute->UnBindFrom(boneWeightIdx); }
	if (normalIdx >= 0) { this->normalAttribute->UnBindFrom(normalIdx); }
	if (positionIdx >= 0) { this->positionAttribute->UnBindFrom(positionIdx); }
	if (textureCoordIdx >= 0) { this->textureCoordAttribute->UnBindFrom(textureCoordIdx); }
}

void Mesh::Draw() {
	if (this->vertexIndices.size() > 0) {
		render::Draw(*this->vertexIndexBuffer, DrawMode::Triangles);
	} else {
		render::Draw(this->positions.size(), DrawMode::Triangles);
	}
}

void Mesh::DrawInstanced(unsigned int numbInstances) {
	if (this->vertexIndices.size() > 0) {
		render::DrawInstanced(*this->vertexIndexBuffer, DrawMode::Triangles, numbInstances);
	} 	else {
		render::DrawInstanced(this->positions.size(), DrawMode::Triangles, numbInstances);
	}
}

}
