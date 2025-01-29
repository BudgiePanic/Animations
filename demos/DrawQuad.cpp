#include "DrawQuad.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include "../Mat4f.h"
#include "../rotation/quaternion.h"
#include "../render/Draw.h"
#include "../render/Uniform.h"
#include <iostream>

namespace demos {

	constexpr float oneDegAsRad = (2.0f * M_PI) / 360.0f;
	const f3 up = f3(0, 1, 0);
	const f3 cameraPosition = f3(0,0,-5);
	const f3 cameraLookingAt = f3();

	void DrawQuad::Initialize() {
		this->rotationRadian = 0.0f;
		std::cout << "Looking for \'\\resource\\shaders\\frag_lit.glsl\' in working directory.\n";
		std::cout << "Looking for \'\\resource\\shaders\\static_vert.glsl\' in working directory.\n";
		this->shader = new render::Shader(
			"./resource/shaders/static_vert.glsl",
			"./resource/shaders/frag_lit.glsl"
		);
		std::cout << "Looking for \'\\resource\\assets\\uv.png\' in working directory.\n";
		this->quadTexture = new render::Texture(
			"./resource/assets/uv.png"
		);
		this->indexBuffer = new render::IndexBuffer();
		this->vertices = new render::Attribute<f3>();
		this->uv = new render::Attribute<f2>();
		this->normals = new render::Attribute<f3>();

		std::vector<f3> verts;
		verts.push_back(f3(-1, -1, 0)); /* 1*--*3  */
		verts.push_back(f3(-1, 1, 0));  /*  |\ |   */
		verts.push_back(f3(1, -1, 0));  /*  | \|   */
		verts.push_back(f3(1, 1, 0));   /* 0*__*2  */
		this->vertices->Set(verts);

		std::vector<f2> uvs;
		uvs.push_back(f2(0, 0));
		uvs.push_back(f2(0, 1));
		uvs.push_back(f2(1, 0));
		uvs.push_back(f2(1, 1));
		this->uv->Set(uvs);

		std::vector<f3> normal;
		normal.resize(4, f3(0, 0, 1));
		this->normals->Set(normal);

		std::vector<unsigned int> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);
		this->indexBuffer->Set(indices);
	}
	void DrawQuad::Update(float deltaTime) {
		this->rotationRadian += deltaTime * oneDegAsRad * 10.0f;
		while (this->rotationRadian > (2.0f * M_PI)) {
			this->rotationRadian -= (2.0f * M_PI);
		}
	}
	void DrawQuad::Render(float aspectRatio) {
		mat4f cameraToScreen = perspective(60.0f, aspectRatio, 0.01f, 1000.0f);
		mat4f worldToCamera = lookAt(cameraPosition, cameraLookingAt, up);
		mat4f modelToWorld = rotation::toMatrix(rotation::angleAxis(this->rotationRadian, f3(0,0,1)));

		this->shader->Bind();
		// Load in attribute data
		bool positionAttribExists;
		unsigned int pos = this->shader->GetAttribute("position", positionAttribExists);
		this->vertices->BindTo(pos);

		bool normalAttribExists;
		unsigned int norm = this->shader->GetAttribute("normal", normalAttribExists);
		this->normals->BindTo(norm);

		bool uvAttribExists;
		unsigned int uvs = this->shader->GetAttribute("textureCoordinate", uvAttribExists);
		this->uv->BindTo(uvs);

		// Load in uniform data
		bool modelUniformExists;
		unsigned int model = this->shader->GetUniform("model_transform", modelUniformExists);
		render::Uniform<mat4f>::Set(model, modelToWorld);

		bool viewUniformExists;
		unsigned int view = this->shader->GetUniform("view_transform", viewUniformExists);
		render::Uniform<mat4f>::Set(view, worldToCamera);

		bool projectionUniformExists;
		unsigned int proj = this->shader->GetUniform("projection", projectionUniformExists);
		render::Uniform<mat4f>::Set(proj, cameraToScreen);

		// Load in data and texture data for the fragment shader
		bool lightUniformExists;
		render::Uniform<f3>::Set(this->shader->GetUniform("sky_light_direction", lightUniformExists), f3(0, 0, 1));

		bool tex0Exists;
		constexpr unsigned int textureIndex = 0;
		this->quadTexture->Set(this->shader->GetUniform("tex0", tex0Exists), textureIndex);

		render::Draw(*this->indexBuffer, render::DrawMode::Triangles);

		// clean up
		this->quadTexture->Unset(textureIndex);
		this->normals->UnBindFrom(norm);
		this->uv->UnBindFrom(uvs);
		this->vertices->UnBindFrom(pos);

		this->shader->Unbind();
	}
	void DrawQuad::ShutDown() {
		delete this->shader;
		delete this->indexBuffer;
		delete this->quadTexture;
		delete this->normals;
		delete this->uv;
		delete this->vertices;
	}
}
