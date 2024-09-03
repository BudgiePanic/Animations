#include "Shader.h"
#include "../glad.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace render {

std::string Shader::ReadFile(const std::string& path)
{
    // (i)nput (f)ile stream
    std::ifstream inStream;
    inStream.open(path);
    if (inStream.bad()) {
        std::cout << "couldn't read file " << path << "\n";
        // program now explodes?
    }
    std::stringstream contents;
    contents << inStream.rdbuf();
    inStream.close();
    return contents.str();
}

unsigned int Shader::CompileVertex(const std::string& vertexSource)
{
    // linking to documentation on what these functions actually do, since MSVC / glad either doesn't have the documentation or won't show it in the editor
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateShader.xhtml
    unsigned int objectHandle = glad_glCreateShader(GL_VERTEX_SHADER);
    if (objectHandle == 0 || objectHandle == GL_INVALID_ENUM) {
        std::cout << "error occured while creating shader\n";
        return 0;
    }
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glShaderSource.xhtml
    // copy the shader source code into the shader object
    const char* source = vertexSource.c_str();
    constexpr int numbOfArrays = 1;
    constexpr GLint* stringIsNullTerminated = NULL;
    glad_glShaderSource(objectHandle, numbOfArrays, &source, stringIsNullTerminated);
    // tell OpenGL to compile the shader source code stored in the shader object
    glad_glCompileShader(objectHandle);
    // Ask OpenGL to check the compilation status
    int status = 0;
    glad_glGetShaderiv(objectHandle, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        constexpr int bufferSize = 512;
        char buffer[bufferSize]{};
        // by providing NULL we tell OpenGL we don't want it to write the log size into any memory location 
        GLsizei *actualLogSize = NULL; 
        glad_glGetShaderInfoLog(objectHandle, bufferSize, actualLogSize, buffer);
        std::cout << "error occured while compiling vertex shader. Releasing shader object\n";
        std::cout << "\t" << buffer << "\n";
        // Tell OpenGL it can reclaim the resources held by the shader object when it feels like it
        glad_glDeleteShader(objectHandle);
        return 0;
    }
    return objectHandle; // someone will need to remember to delete the shader objects later
}

unsigned int Shader::CompileFragment(const std::string& fragmentSource)
{
    // I don't know why the book author didn't just make a generic method to compile shaders
    // where you pass the shader type? "production ready code included" lol
    unsigned int objectHandle = glad_glCreateShader(GL_FRAGMENT_SHADER);
    if (objectHandle == 0 || objectHandle == GL_INVALID_ENUM) {
        std::cout << "error occured while creating shader\n";
        return 0;
    }
    const char* source = fragmentSource.c_str();
    constexpr int numbOfArrays = 1;
    constexpr GLint* stringIsNullTerminated = NULL;
    glad_glShaderSource(objectHandle, numbOfArrays, &source, stringIsNullTerminated);
    glad_glCompileShader(objectHandle);
    int status = 0;
    glad_glGetShaderiv(objectHandle, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        constexpr int bufferSize = 512;
        char buffer[bufferSize]{}; 
        GLsizei* actualLogSize = NULL;
        glad_glGetShaderInfoLog(objectHandle, bufferSize, actualLogSize, buffer);
        std::cout << "error occured while compiling fragment shader. Releasing shader object\n";
        std::cout << "\t" << buffer << "\n";
        glad_glDeleteShader(objectHandle);
        return 0;
    }
    return objectHandle; // someone will need to remember to delete the shader objects later
}

bool Shader::LinkShaders(unsigned int vertex, unsigned int fragment)
{
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glAttachShader.xhtml
    glad_glAttachShader(this->mHandle, vertex);
    glad_glAttachShader(this->mHandle, fragment);
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glLinkProgram.xhtml
    glad_glLinkProgram(this->mHandle);
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgram.xhtml
    int linkStatus = 0;
    glad_glGetProgramiv(this->mHandle, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        constexpr int bufferSize = 512;
        char buffer[bufferSize]{};
        GLsizei* actualLogSize = NULL;
        glad_glGetProgramInfoLog(this->mHandle, bufferSize, actualLogSize, buffer);
        std::cout << "error occured while linking shaders. Releasing shader objects\n";
        std::cout << "\t" << buffer << "\n";
    }
    glad_glDeleteShader(vertex);
    glad_glDeleteShader(fragment);
    return linkStatus == GL_TRUE;
}

Shader::Shader()
{
    this->mHandle = glad_glCreateProgram();
    if (mHandle == 0) {
        std::cout << "error while creating shader program\n";
    }
}

Shader::Shader(const std::string& vertex, const std::string& fragment)
{
    this->mHandle = glad_glCreateProgram();
    if (mHandle == 0) {
        std::cout << "error while creating shader program\n";
    }
    this->Load(vertex, fragment);
}

Shader::~Shader()
{
    glad_glDeleteProgram(this->mHandle);
}

void Shader::Load(const std::string& vertex, const std::string& fragment)
{
}

void Shader::Bind()
{
}

void Shader::Unbind()
{
}

unsigned int Shader::GetAttribute(const std::string& name)
{
    return 0;
}

unsigned int Shader::GetUniform(const std::string& name)
{
    return 0;
}

unsigned int Shader::GetHandle()
{
    return 0;
}

}

