#pragma once

namespace render{

class Texture {
protected:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mChannels;
	unsigned int mFileHandle;
private:
	// Prevent aliasing of texture resources by hiding copy constructor and assignment operator
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);

public:
	Texture();
	Texture(const char* filePath);
	~Texture();
	/// <summary>
	/// TODO
	/// </summary>
	/// <param name="filePath"></param>
	void Load(const char* filePath);
	/// <summary>
	/// TODO
	/// </summary>
	/// <param name="uniform"></param>
	/// <param name="textureIndex"></param>
	void Set(unsigned int uniform, unsigned int textureIndex);
	/// <summary>
	/// TODO
	/// </summary>
	/// <param name="textureIndex"></param>
	void Unset(unsigned int textureIndex);
	/// <summary>
	/// TODO
	/// </summary>
	/// <returns></returns>
	unsigned int GetHandle();
};

}
