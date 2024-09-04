#pragma once

#include <vector>

namespace render {

/// <summary>
/// Abstraction class to hide the details of sending attributes to the GPU through the OpenGL library.
/// </summary>
/// <typeparam name="t">The type of attribute data, i.e. vec3, vec2, etc.</typeparam>
template<typename t>
class Attribute {
protected:
	/// <summary>
	/// The OpenGL handle to the attribute resource.
	/// </summary>
	unsigned int mHandle;
	/// <summary>
	/// The amount of data being stored in this attribute.
	/// </summary>
	unsigned int mCount;
private:
	/// <summary>
	/// Hide the copy constructor to prevent multiple objects aliasing the same OpenGL resources.
	/// </summary>
	/// <param name="other"></param>
	Attribute(const Attribute& other);
	/// <summary>
	/// Hide the assignment operator to prevent aliasing of OpenGL resources.
	/// </summary>
	Attribute& operator=(const Attribute& other);
	/// <summary>
	/// 
	/// </summary>
	/// <param name="slot"></param>
	void SetAttributePointer(unsigned int slot);
public:
	Attribute();
	~Attribute();
	/// <summary>
	/// Upload data to the GPU
	/// </summary>
	/// <param name="inputArray"></param>
	/// <param name="arrayLength"></param>
	void Set(t* dataArray, unsigned int arrayLength);
	/// <summary>
	/// Upload data to the GPU.
	/// </summary>
	/// <param name="data"></param>
	void Set(std::vector<t>& data);
	/// <summary>
	/// TODO
	/// </summary>
	/// <param name="slot"></param>
	void BindTo(unsigned int slot);
	/// <summary>
	/// TODO
	/// </summary>
	/// <param name="slot"></param>
	void UnBindFrom(unsigned int slot);
	/// <summary>
	/// Get the count of data stored in this attribute.
	/// </summary>
	/// <returns></returns>
	unsigned int Count();
	/// <summary>
	/// Get the OpenGL handle for this attribute resource.
	/// </summary>
	/// <returns></returns>
	unsigned int GetHandle();
};

}
