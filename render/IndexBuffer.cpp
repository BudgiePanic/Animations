#include "IndexBuffer.h"
#include "../glad.h"

namespace render{

IndexBuffer::IndexBuffer() {
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenBuffers.xhtml 
	glad_glGenBuffers(1, &this->mHandle); // write the handle of the buffer into 'this' handle
}

IndexBuffer::~IndexBuffer() {
	glad_glDeleteBuffers(1, &this->mHandle);
}

void IndexBuffer::Set(unsigned int* intArray, unsigned int arrayLength) {
	this->mCount = arrayLength;
	unsigned int bufferSize = arrayLength * sizeof(unsigned int);
	glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mHandle);
	glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, intArray, GL_STATIC_DRAW);
	glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Set(std::vector<unsigned int>& data) {
	Set(&data[0], (unsigned int) data.size());
}

unsigned int IndexBuffer::Count() {
	return this->mCount;
}

unsigned int IndexBuffer::GetHandle() {
	return this->mHandle;
}

}
