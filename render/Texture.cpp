#include "Texture.h"
#include "../glad.h"
#include "../stb_image.h"
#include <iostream>

namespace render {
	
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenTextures.xhtml 

	Texture::Texture() {
		this->mChannels = 0;
		glad_glGenTextures(1, &this->mTextureHandle);
		this->mWidth = 0;
		this->mHeight = 0;
	}

	Texture::Texture(const char* filePath) {
		glad_glGenTextures(1, &this->mTextureHandle);
		this->Load(filePath);
	}

	Texture::~Texture() {
		glad_glDeleteTextures(1, &this->mTextureHandle);
	}

	void Texture::Load(const char* filePath) {
		int width, height, channels;
		unsigned char* data = stbi_load(filePath, &width, &height, &channels, 4); // 4 >> RGBA
		if (data == NULL) {
			std::cout << "couldn't load image at " << filePath << "\n";
			return;
		}
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
		constexpr int lodBaseLevel = 0, noBorder = 0;
		glad_glTexImage2D(
			GL_TEXTURE_2D,
			lodBaseLevel,
			GL_RGBA,
			width,
			height,
			noBorder,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			data
		);
		stbi_image_free(data);
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenerateMipmap.xhtml
		glad_glGenerateMipmap(GL_TEXTURE_2D);
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
		glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glad_glBindTexture(GL_TEXTURE_2D, 0);
		this->mChannels = channels;
		this->mHeight = height;
		this->mWidth = width;
	}

	void Texture::Set(unsigned int uniform, unsigned int textureIndex) {
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glActiveTexture.xhtml 
		glad_glActiveTexture(GL_TEXTURE0 + textureIndex);
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindTexture.xhtml
		glad_glBindTexture(GL_TEXTURE_2D, this->mTextureHandle);
		// load the texture data into the uniform as a 'sampler type' 
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml#notes
		glad_glUniform1i(uniform, textureIndex);
	}

	void Texture::Unset(unsigned int textureIndex) {
		glad_glActiveTexture(GL_TEXTURE0 + textureIndex);
		glad_glBindTexture(GL_TEXTURE_2D, 0);
		glad_glActiveTexture(GL_TEXTURE0);
	}

	unsigned int Texture::GetHandle() {
		return this->mTextureHandle;
	}

}
