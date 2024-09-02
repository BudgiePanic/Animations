#pragma once
#include "IndexBuffer.h"

namespace render {

enum class DrawMode {
	Points,
	LineStrip,
	LineLoop,
	Lines,
	Triangles, 
	TriangleStrip,
	TriangleFan
};

/*
	GLOBAL RENDERING FUNCTIONS
*/

/// <summary>
/// TODO
/// </summary>
/// <param name="indexBuffer"></param>
/// <param name="mode"></param>
void Draw(IndexBuffer& indexBuffer, DrawMode mode);
/// <summary>
/// TODO
/// </summary>
/// <param name="vertexCount"></param>
/// <param name="mode"></param>
void Draw(unsigned int vertexCount, DrawMode mode);

/// <summary>
/// TODO
/// </summary>
/// <param name="indexBuffer"></param>
/// <param name="mode"></param>
/// <param name="instanceCount"></param>
void DrawInstanced(IndexBuffer& indexBuffer, DrawMode mode, unsigned int instanceCount);

/// <summary>
/// TODO
/// </summary>
/// <param name="vertexCount"></param>
/// <param name="mode"></param>
/// <param name="numInstances"></param>
void DrawInstanced(unsigned int vertexCount, DrawMode mode, unsigned int numInstances);

}
