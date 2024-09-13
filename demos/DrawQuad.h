#pragma once
#include "../Application.h"
#include "../render/Shader.h"
#include "../render/Attribute.h"
#include "../Vector3.h"
#include "../Vector2.h"
#include "../render/IndexBuffer.h"
#include "../render/Texture.h"

namespace demos {
	
	class DrawQuad : public Application {
	protected:
		render::Shader* shader;
		render::Attribute<f3>* vertices;
		render::Attribute<f3>* normals;
		render::Attribute<f2>* uv;
		render::IndexBuffer* indexBuffer;
		render::Texture* quadTexture;
		float rotationRadian;
	public:
		void Initialize();
		void ShutDown();
		void Update(float deltaTime);
		void Render(float aspectRatio);
	};

}
