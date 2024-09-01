#pragma once
#include <vector>

/*
	Index Buffer Attribute abstraction class.
*/

namespace render {

class IndexBuffer {
public:
	/// <summary>
	/// OpenGL provided reference to the index buffer object.
	/// </summary>
	unsigned int mHandle;
	/// <summary>
	/// How much stuff is stored in the buffer.
	/// </summary>
	unsigned int mCount;
private:
	// Hide the copy constructor and assignment operator to
	// prevent multiple buffers having references to the same index buffer objects.
	IndexBuffer(const IndexBuffer& other);
	IndexBuffer& operator=(const IndexBuffer& other);
public:
	IndexBuffer();
	~IndexBuffer();
	/// <summary>
	/// Send integer array data to the GPU.
	/// </summary>
	/// <param name="intArray">The data to send</param>
	/// <param name="arrayLength">The size of the array</param>
	void Set(unsigned int* intArray, unsigned int arrayLength);
	void Set(std::vector<unsigned int>& data);
	unsigned int Count();
	unsigned int GetHandle();
};

}
