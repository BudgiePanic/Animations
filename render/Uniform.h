#pragma once

#include <vector>

namespace render {

/// <summary>
/// Abstraction layer for OpenGL uniform managment.
/// A uniform is a piece of data, or buffer of data, used in a shader program that is constant for all vertices.
/// A classic use of a uniform is to store the virtual camera's view matrix when converting vertices from world space to camera space.
/// The uniform class provides only static methods and cannot be instantiated.
/// </summary>
/// <typeparam name="T">The data type of the uniform</typeparam>
template<typename T>
class Uniform {
// static only class, hide all constructors
private:
	Uniform();
	Uniform(const Uniform&);
	Uniform& operator=(const Uniform&);
	~Uniform();
public:
	/// <summary>
	/// Upload a single piece of data to the GPU.
	/// </summary>
	/// <param name="slot">The (uniform buffer?) that the data is sent to</param>
	/// <param name="datum">The data to send to the GPU</param>
	static void Set(unsigned int slot, const T& datum);
	/// <summary>
	/// Upload multiple pieces of data to a uniform on the GPU.
	/// </summary>
	/// <param name="slot"></param>
	/// <param name="dataArray"></param>
	/// <param name="arrayLength"></param>
	static void Set(unsigned int slot, const T* dataArray, unsigned int arrayLength);
	/// <summary>
	/// Upload multiple pieces of data to a uniform on the GPU.
	/// </summary>
	/// <param name="slot"></param>
	/// <param name="data"></param>
	static void Set(unsigned int slot, std::vector<T>& data);
};

}
