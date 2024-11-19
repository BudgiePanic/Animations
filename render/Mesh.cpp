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
	// write the animated pose bone's into the mat4f vector
	animatedPose.ToMatrixPalette(this->posedBones);
	std::vector<mat4f> invBindPose = skeleton.GetInverseBindPose();
	// skinning every vertex
	for (int i = 0; i < vertCount; i++) {
		// bones that influence this vertex
		i4& bones = this->boneIndices[i];
		// how much each bone affects this vertex (if a bone has no influence, its weight will be zero)
		f4& weights = this->boneWeights[i];
		// create the skinning matrix
		mat4f skinningTransform0 = (this->posedBones[bones.x] * invBindPose[bones.x]) * weights.x;
		mat4f skinningTransform1 = (this->posedBones[bones.y] * invBindPose[bones.y]) * weights.y;
		mat4f skinningTransform2 = (this->posedBones[bones.z] * invBindPose[bones.z]) * weights.z;
		mat4f skinningTransform3 = (this->posedBones[bones.w] * invBindPose[bones.w]) * weights.w;
		// combined matrix that skins to all of the bones
		mat4f skinningTransform = skinningTransform0 + skinningTransform1 + skinningTransform2 + skinningTransform3;
		this->skinnedPositions[i] = multiplyPoint(skinningTransform, this->positions[i]);
		this->skinnedNormals[i] = multiplyVector(skinningTransform, this->normals[i]);
	}
	// tell the GPU about the work we've done
	this->positionAttribute->Set(this->skinnedPositions);
	this->normalAttribute->Set(this->skinnedNormals);
}

void Mesh::Skin(std::vector<mat4f>& posedBones) {
	unsigned int numbVerts = this->positions.size();
	if (numbVerts == 0) { return; } // nothing to skin
	this->skinnedPositions.resize(numbVerts);
	this->skinnedNormals.resize(numbVerts);
	// skin every vertex
	for (unsigned int i = 0; i < numbVerts; i++) {
		i4& bones = this->boneIndices[i]; // which bones affect this vertex
		f4& weights = this->boneWeights[i]; // how much the bones affect the vertex. Invalid bones will have a weight of zero
		f3& vertex = this->positions[i];
		f3 posedVertX = multiplyPoint(posedBones[bones.x], vertex);
		f3 posedVertY = multiplyPoint(posedBones[bones.y], vertex);
		f3 posedVertZ = multiplyPoint(posedBones[bones.z], vertex);
		f3 posedVertW = multiplyPoint(posedBones[bones.w], vertex);
		this->skinnedPositions[i] = (posedVertX * weights.x) +
									(posedVertY * weights.y) +
									(posedVertZ * weights.z) +
									(posedVertW * weights.w);
		f3& normal = this->normals[i];
		f3 posedNormX = multiplyVector(posedBones[bones.x], normal);
		f3 posedNormY = multiplyVector(posedBones[bones.y], normal);
		f3 posedNormZ = multiplyVector(posedBones[bones.z], normal);
		f3 posedNormW = multiplyVector(posedBones[bones.w], normal);
		this->skinnedNormals[i] = (posedNormX * weights.x) +
								  (posedNormY * weights.y) +
								  (posedNormZ * weights.z) +
								  (posedNormW * weights.w);
	}
	this->positionAttribute->Set(this->skinnedPositions);
	this->normalAttribute->Set(this->skinnedNormals);
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
