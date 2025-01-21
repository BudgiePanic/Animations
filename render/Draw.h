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
/// Binds to the provided index buffer then issues a draw call to the GPU.
/// </summary>
/// <param name="indexBuffer"></param>
/// <param name="mode"></param>
void Draw(IndexBuffer& indexBuffer, DrawMode mode);
/// <summary>
/// Issues a draw call to the GPU for a provided number of vertices in the currently bound array.
/// </summary>
/// <param name="vertexCount"></param>
/// <param name="mode"></param>
void Draw(unsigned int vertexCount, DrawMode mode);

/// <summary>
/// Binds to the provided index buffer then issues an instanced draw elements call.
/// </summary>
/// <param name="indexBuffer"></param>
/// <param name="mode"></param>
/// <param name="instanceCount"></param>
void DrawInstanced(IndexBuffer& indexBuffer, DrawMode mode, unsigned int instanceCount);

/// <summary>
/// Issues an instanced array draw call.
/// </summary>
/// <param name="vertexCount"></param>
/// <param name="mode"></param>
/// <param name="numInstances"></param>
void DrawInstanced(unsigned int vertexCount, DrawMode mode, unsigned int numInstances);

/// <summary>
/// Enable or disable the depth buffer
/// </summary>
/// <param name="state">pass true to enable the depth buffer, false to disable the depth buffer</param>
void SetDepthBuffer(bool state);

}
