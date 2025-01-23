#include "Uniform.h"
#include "../Vector2.h"
#include "../Vector3.h"
#include "../Vector4.h"
#include "../rotation/quaternion.h"
#include "../glad.h"
#include "../Mat4f.h"
#include "../transforms/DualQuaternion.h"

/*#define UNIFORM_SET_IMPL_TYPE(gl_function, containerType, dataType) \
template<> void Uniform<containerType>::Set( \
unsigned int slot, const containerType* data, unsigned int arrayLength) { \
gl_function(slot, (GLsizei)arrayLength, (dataType*)&data[0]); \
}
*/

namespace render {
	
	template Uniform<int>;
	template Uniform<i4>;
	template Uniform<i2>;
	template Uniform<float>;
	template Uniform<f2>;
	template Uniform<f3>;
	template Uniform<rotation::quaternion>;
	template Uniform<f4>;
	template Uniform<mat4f>;
	template Uniform<transforms::DualQuaternion>;


	template <typename T>
	void Uniform<T>::Set(unsigned int slot, const T& datum) {
		Set(slot, (T*)&datum, 1);
	}

	template <typename T>
	void Uniform<T>::Set(unsigned int slot, std::vector<T>& data) {
		Set(slot, &data[0], (unsigned int)data.size());
	}

	// macro generated functions
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml 
	template<> void Uniform<int>::Set(unsigned int slot, const int* data, unsigned int arrayLength) {
		glad_glUniform1iv(slot, (GLsizei)arrayLength, (int*)&data[0]);
	}	

	template<> void Uniform<i4>::Set(unsigned int slot, const i4* data, unsigned int arrayLength) {
		glad_glUniform4iv(slot, (GLsizei)arrayLength, (int*)&data[0]);
	}	

	template<> void Uniform<i2>::Set(unsigned int slot, const i2* data, unsigned int arrayLength) {
		glad_glUniform2iv(slot, (GLsizei)arrayLength, (int*)&data[0]);
	}	

	template<> void Uniform<float>::Set(unsigned int slot, const float* data, unsigned int arrayLength) {
		glad_glUniform1fv(slot, (GLsizei)arrayLength, (float*)&data[0]);
	}	

	template<> void Uniform<f2>::Set(unsigned int slot, const f2* data, unsigned int arrayLength) {
		glad_glUniform2fv(slot, (GLsizei)arrayLength, (float*)&data[0]);
	}	

	template<> void Uniform<f3>::Set(unsigned int slot, const f3* data, unsigned int arrayLength) {
		glad_glUniform3fv(slot, (GLsizei)arrayLength, (float*)&data[0]);
	}	

	template<> void Uniform<f4>::Set(unsigned int slot, const f4* data, unsigned int arrayLength) {
		glad_glUniform4fv(slot, (GLsizei)arrayLength, (float*)&data[0]);
	}	

	template<> void Uniform<rotation::quaternion>::Set(unsigned int slot, const rotation::quaternion* data, unsigned int arrayLength) {
		glad_glUniform4fv(slot, (GLsizei)arrayLength, (float*)&data[0]);
	}	

	template<> void Uniform<mat4f>::Set(unsigned int slot, const mat4f* data, unsigned int arrayLength) {
		constexpr bool shouldTransposeMatrix = false;
		glad_glUniformMatrix4fv(slot, (GLsizei) arrayLength, shouldTransposeMatrix, (float*)&data[0]);
	}

	template<> void Uniform<transforms::DualQuaternion>::Set(unsigned int slot, const transforms::DualQuaternion* data, unsigned int arrayLength) {
		constexpr bool shouldTransposeMatrix = false;
		glad_glUniformMatrix2x4fv(slot, arrayLength, shouldTransposeMatrix, data[0].v);
	}
}
