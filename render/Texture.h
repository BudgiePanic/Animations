#pragma once

namespace render{

class Texture {
protected:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mChannels;
	unsigned int mTextureHandle;
private:
	// Prevent aliasing of texture resources by hiding copy constructor and assignment operator
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);

public:
	Texture();
	Texture(const char* filePath);
	~Texture();
	/// <summary>
	/// Load texture from file using stbi, allocate space on GPU for texture, and upload texture to GPU.
	/// </summary>
	/// <param name="filePath"></param>
	void Load(const char* filePath);
	/// <summary>
	/// Sets this texture to be affected by subsequent texture calls. Binds the texture to a texture unit, and provides the texture unit to the provided uniform.
	/// </summary>
	/// <param name="uniform"></param>
	/// <param name="textureIndex"></param>
	void Set(unsigned int uniform, unsigned int textureIndex);
	/// <summary>
	/// Undoes the operations of 'Set' by making 'this' texture active, then unbinding the texture unit it was bound to, then sets the active texture to default.
	/// </summary>
	/// <param name="textureIndex"></param>
	void Unset(unsigned int textureIndex);
	/// <summary>
	/// Get the OpenGL name associated with 'this' texture.
	/// </summary>
	/// <returns></returns>
	unsigned int GetHandle();
};

}
