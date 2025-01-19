#include "Draw.h"
#include "../glad.h"

namespace render {

	static GLenum ToGLenum(DrawMode mode) {
		switch (mode) {
		case DrawMode::Points:
			return GL_POINTS;
		case DrawMode::LineStrip:
			return GL_LINE_STRIP;
		case DrawMode::TriangleStrip:
			return GL_TRIANGLE_STRIP;
		case DrawMode::Triangles:
			return GL_TRIANGLES;
		case DrawMode::TriangleFan:
			return GL_TRIANGLE_FAN;
		case DrawMode::LineLoop:
			return GL_LINE_LOOP;
		case DrawMode::Lines:
			return GL_LINES;
		}
	}

	void Draw(IndexBuffer& indexBuffer, DrawMode mode) {
		unsigned int handle = indexBuffer.GetHandle();
		unsigned int numIndices = indexBuffer.Count();
		glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		// render primitives using the bound shader program
		glad_glDrawElements(
			ToGLenum(mode), // type of primitives being rendered
			numIndices,		// number of elements to render, elements will be found in the enabled array, initial offset into the array is the last parameter of this function
			GL_UNSIGNED_INT,// the type of the value in the bound buffer?
			0 // offset into the enabled array to where the indices are
		);
		glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Draw(unsigned int vertexCount, DrawMode mode) {
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
		glad_glDrawArrays(
			ToGLenum(mode), // type of primative being rendered
			0,				// index of starting array
			vertexCount     // number of entries in the array to render
		);
	}

	void DrawInstanced(IndexBuffer& indexBuffer, DrawMode mode, unsigned int instanceCount) {
		unsigned int handle = indexBuffer.GetHandle();
		unsigned int numIndices = indexBuffer.Count();
		glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		glad_glDrawElementsInstanced(
			ToGLenum(mode), 
			numIndices, 
			GL_UNSIGNED_INT, 
			0, 
			instanceCount
		);
		glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void DrawInstanced(unsigned int vertexCount, DrawMode mode, unsigned int numInstances) {
		glad_glDrawArraysInstanced(
			ToGLenum(mode),
			0, // start index in the enabled arrays
			vertexCount, // number of indices to render
			numInstances // Specifies the number of instances of the specified range of indices to be rendered
		);
	}

	void SetDepthBuffer(bool state)	{
		if (state) {
			glad_glEnable(GL_DEPTH_TEST);
		} else {
			glad_glDisable(GL_DEPTH_TEST);
		}
	}

}

