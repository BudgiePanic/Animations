#pragma once
#include <map>
#include <string>

namespace render {

/// <summary>
/// OpenGL shader abstraction object. Hides the OpenGL/system calls.
/// Compiles vertex and fragment shaders. Provides methods for interacting with shaders in memory
/// </summary>
class Shader {
private:
	/// <summary>
	/// The handle is an OpenGL library provided reference to the shader program in memory (reference != address).
	/// The shader program is a binary object that can be sent to the GPU for execution.
	/// </summary>
	unsigned int mHandle;
	/// <summary>
	/// shader attributes are 'per vertex' inputs to the shader program, such as vertex position, vertex normal, and vertex color.
	/// shaders can only accept a finite number of input attributes.
	/// </summary>
	std::map<std::string, unsigned int> mAttributes;
	/// <summary>
	/// shader uniforms are global pieces of data accessible from every shader in the shader program where they are declared.
	/// A shader program consists of linked shaders capable of performing the entire graphics pipeline.
	/// </summary>
	std::map<std::string, unsigned int> mUniforms;
private:
	/// <summary>
	/// Hide the copy constructor to prevent shader aliasing.
	/// </summary>
	/// <param name=""></param>
	Shader(const Shader&);
	/// <summary>
	/// Hide the shader assignment operator to prevent shader aliasing.
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Shader& operator=(const Shader&);
private:
	/// <summary>
	/// Read a file that contains shader source code.
	/// </summary>
	/// <param name="path"></param>
	/// <returns>The file contents as a string</returns>
	std::string ReadFile(const std::string& path);
	/// <summary>
	/// Ask the OpenGL library to compile vertex shader code into a linkable object.
	/// https://www.khronos.org/opengl/wiki/Shader_Compilation
	/// </summary>
	/// <param name="vertexSource">The source code of the shader</param>
	/// <returns>A handle to the compiled shader object</returns>
	unsigned int CompileVertex(const std::string& vertexSource);
	/// <summary>
	/// Ask the OpenGL library to compile fragment shader code into a linkable object.
	/// </summary>
	/// <param name="fragmentSource">The source code of the fragment shader</param>
	/// <returns>A handle to the compiled shader object</returns>
	unsigned int CompileFragment(const std::string& fragmentSource);
	/// <summary>
	/// Asks the OpenGL library to link a vertex shader object and fragment shader object together to form a GPU executable shader program.
	/// </summary>
	/// <param name="vertex">the handle of the vertex object</param>
	/// <param name="fragment">the handle of the fragment object</param>
	/// <returns>Whether the linking process succeeded</returns>
	bool LinkShaders(unsigned int vertex, unsigned int fragment);
	/// <summary>
	/// Ask OpenGL to get attribute indices for the input attributes for this shader program.
	/// The attribute index is used to specify which attribute data should be sent to.
	/// </summary>
	void PopulateAttributes();
	/// <summary>
	/// Get the indices of the uniforms used in this shader program.
	/// </summary>
	void PopulateUniforms();

public:
	/// <summary>
	/// Create a shader object. Part of object initialization is asking OpenGL to allocate resources for an empty shader program.
	/// </summary>
	Shader();
	/// <summary>
	/// Create a shader object, allocate shader program resources. Compile shader code and link or load shader source from file, compile and link (parameter types determined at runtime).
	/// </summary>
	/// <param name="vertex">Either the path of a vertex source file, or inline vertex source code</param>
	/// <param name="fragment">Either the path of a fragment source file, or inline fragment source code</param>
	Shader(const std::string& vertex, const std::string& fragment);
	/// <summary>
	/// Releases the resources that OpenGL allocated for this shader object prior to destruction.
	/// </summary>
	~Shader();
	/// <summary>
	/// Compiles and links fragment and vertex shader code into a GPU executable shader program.
	/// Dynamically determines if function parameters are inline shader source code or file paths to shader source code.
	/// Loads shader source from file if path is provided instead of inline source code.
	/// </summary>
	/// <param name="vertex">Either the path of a vertex source file, or inline vertex source code</param>
	/// <param name="fragment">Either the path of a fragment source file, or inline fragment source code</param>
	void Load(const std::string& vertex, const std::string& fragment);
	/// <summary>
	/// Ask OpenGL to use the shader program encapsulated by this shader object on the GPU.
	/// </summary>
	void Bind();
	/// <summary>
	/// Tell OpenGL to stop using the shader program encapsulated by this shader object on the GPU.
	/// </summary>
	void Unbind();
	/// <summary>
	/// Get a shader attribute, if it exists, from the shader program.
	/// </summary>
	/// <param name="name"></param>
	/// <param name="exists">Where to write the query result</param>
	/// <returns></returns>
	unsigned int GetAttribute(const std::string& name, bool& exists);
	/// <summary>
	/// Get a shader uniform from the shader program, if it exists.
	/// </summary>
	/// <param name="name"></param>
	/// <param name="exists">Where to write the query result</param>
	/// <returns></returns>
	unsigned int GetUniform(const std::string& name, bool& exists);
	/// <summary>
	/// Get the OpenGL handle for this shader's object.
	/// </summary>
	/// <returns>The handle to the shader program associated with this shader.</returns>
	unsigned int GetHandle();
};

}
