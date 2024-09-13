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

void Shader::PopulateAttributes()
{
    glad_glUseProgram(this->mHandle); // book author doesn't explain why we have to set active shader program here.
    int numbAttributes = -1;
    glad_glGetProgramiv(this->mHandle, GL_ACTIVE_ATTRIBUTES, &numbAttributes);
    constexpr int maxNameSize = 128; // I don't know what would happen if the attribute name was > 128 characters.
    for (int i = 0; i < numbAttributes; i++) {
        char attribName[maxNameSize]{};
        GLenum attibuteDataType;
        int attribNameLength;
        int attribDataSize;
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetActiveAttrib.xhtml
        glad_glGetActiveAttrib(
            this->mHandle, 
            (GLuint) i, 
            maxNameSize, // Tell OpenGL how big the name buffer is
            &attribNameLength,
            &attribDataSize,
            &attibuteDataType,
            attribName
        );
        int attribIndex = glad_glGetAttribLocation(this->mHandle, attribName);
        if (attribIndex >= 0) {
            this->mAttributes[std::string(attribName)] = attribIndex;
        }
    }
    glad_glUseProgram(0);
}

void Shader::PopulateUniforms()
{
    glad_glUseProgram(this->mHandle);
    int numbUniforms = -1;
    glad_glGetProgramiv(this->mHandle, GL_ACTIVE_UNIFORMS, &numbUniforms);
    constexpr int maxNameSize = 128; 
    for (int i = 0; i < numbUniforms; i++) {
        char uniformName[maxNameSize]{};
        GLenum uniformDataType;
        int uniformNameLength;
        int uniformDataSize;
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
        glad_glGetActiveUniform(
            this->mHandle,
            (GLuint)i,
            maxNameSize,
            &uniformNameLength,
            &uniformDataSize,
            &uniformDataType,
            uniformName
        );
        int uniformLocation = glad_glGetUniformLocation(this->mHandle, uniformName);
        if (uniformLocation >= 0) {
            // check if the uniform is an array
            std::string name(uniformName);
            std::size_t position = name.find('[');
            if (position != std::string::npos) {
                // if it is an array, we need to get the uniform locations for each array index
                // i.e. v[0], v[1], v[2], etc, etc.
                name.erase(name.begin() + position, name.end()); // chop off the array index part of the name[]
                for (unsigned int index = 0;;index++) {
                    char uniformArrName[256]{}; // not sure why the author chose 256
                    sprintf_s(uniformArrName, "%s[%d]", name.c_str(), index);
                    int uniformArrLocation = glad_glGetUniformLocation(this->mHandle, uniformArrName);
                    if (uniformArrLocation < 0) {
                        // invalid array index reached
                        break;
                    }
                    this->mUniforms[std::string(uniformArrName)] = uniformArrLocation;
                }
            }
            this->mUniforms[name] = uniformLocation;
        }
    }
    glad_glUseProgram(0);
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
    // determine if vertex is inline source code or a file path
    std::ifstream file(vertex.c_str());
    bool isVertexFile = file.good();
    file.close();
    // determine if fragment is inline source code or a file path
    file = std::ifstream(fragment.c_str());
    bool isFragmentFile = file.good();
    file.close();
    std::string vertexSource = isVertexFile ? ReadFile(vertex) : vertex;
    std::string fragmentSource = isFragmentFile ? ReadFile(fragment) : fragment;
    // compile and link
    unsigned int vertexObjectHandle = this->CompileVertex(vertexSource);
    unsigned int fragmentObjectHandle = this->CompileFragment(fragmentSource);
    bool compiled = this->LinkShaders(vertexObjectHandle, fragmentObjectHandle);
    if (compiled) {
        this->PopulateAttributes();
        this->PopulateUniforms();
    }
}

void Shader::Bind()
{
    glad_glUseProgram(this->mHandle);
}

void Shader::Unbind()
{
    glad_glUseProgram(0);
}

unsigned int Shader::GetAttribute(const std::string& name, bool& exists)
{
    auto iterator = this->mAttributes.find(name);
    if (iterator == this->mAttributes.end()) {
        // no attribute with the provided name exists
        exists = false;
        return 0;
    }
    exists = true;
    return iterator->second; // iterator contains key, value pairs
}

unsigned int Shader::GetUniform(const std::string& name, bool& exists)
{
    auto iterator = this->mUniforms.find(name);
    if (iterator == this->mUniforms.end()) {
        exists = false;
        return 0;
    }
    exists = true;
    return iterator->second; // iterator contains key, value pairs
}

unsigned int Shader::GetHandle()
{
    return this->mHandle;
}

}

