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
