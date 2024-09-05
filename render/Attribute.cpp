#include "Attribute.h"
#include "../Vector2.h"
#include "../Vector3.h"
#include "../Vector4.h"
#include "../glad.h"

namespace render {
	template Attribute<int>;
	template Attribute<float>;
	template Attribute<f2>;
	template Attribute<f3>;
	template Attribute<f4>;
	template Attribute<i4>;

	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
	template<>
	void Attribute<int>::SetAttributePointer(unsigned int slot) {
		// how should OpenGL interpret the data in the buffer
		glad_glVertexAttribIPointer(
		slot,		// attibute slot index
		1,			// number of components in the vertex attribute i.e. RGBA
		GL_INT,		// data type of the components
		0,			// byte offset for consectutive array entries i.e. how many bytes to jump to reach data for vertex at (index+1)
		(void*)NULL	// position offset for start of data in buffer
		);
	}

	template<>
	void Attribute<i4>::SetAttributePointer(unsigned int slot) {
		glad_glVertexAttribIPointer(
			slot,		
			4,			
			GL_INT,		
			0,			
			(void*)NULL	
		);
	}

	template<>
	void Attribute<float>::SetAttributePointer(unsigned int slot) {
		glad_glVertexAttribPointer(
			slot,
			1,
			GL_FLOAT,
			GL_FALSE, // should OpenGL normalize the float value?
			0,
			(void*)NULL
		);
	}


	template<>
	void Attribute<f2>::SetAttributePointer(unsigned int slot) {
		glVertexAttribPointer(
			slot,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)NULL
		);
	}

	template<>
	void Attribute<f3>::SetAttributePointer(unsigned int slot) {
		glVertexAttribPointer(
			slot,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)NULL
		);
	}

	template<>
	void Attribute<f4>::SetAttributePointer(unsigned int slot) {
		glVertexAttribPointer(
			slot,
			4,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)NULL
		);
	}

	template<typename T>
	Attribute<T>::Attribute() {
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGenBuffers.xhtml 
		glad_glGenBuffers(1, &this->mHandle);
		this->mCount = 0;
	}

	template<typename T>
	Attribute<T>::~Attribute() {
		glad_glDeleteBuffers(1, &this->mHandle);
		this->mCount = 0;
	}

	template<typename t>
	void Attribute<t>::Set(t* dataArray, unsigned int arrayLength) {
		// Tell OpenGL we want to send data to a buffer
		glad_glBindBuffer(GL_ARRAY_BUFFER, this->mHandle);
		this->mCount = arrayLength;
		unsigned int size = sizeof(t);
		// Tell OpenGL to send the data to the buffer
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml
		// check the website for explaination of the meaning of the symbolic constants
		glad_glBufferData(
			GL_ARRAY_BUFFER,		// The supplied data are vertex attribute data
			size * this->mCount,	// the size of the data to store
			dataArray,				// data location
			GL_STREAM_DRAW			// give OpenGL a hint as to how we intent to access the buffer (access frequency + read? or read/write?)
		);
		glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template<typename t>
	void Attribute<t>::Set(std::vector<t>& data) {
		this->Set(&data[0], (unsigned int)data.size());
	}

	template<typename t>
	void Attribute<t>::BindTo(unsigned int slot)
	{
		glad_glBindBuffer(GL_ARRAY_BUFFER, this->mHandle);
		glad_glEnableVertexAttribArray(slot);
		this->SetAttributePointer(slot);
		glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template<typename t>
	void Attribute<t>::UnBindFrom(unsigned int slot) {
		glad_glBindBuffer(GL_ARRAY_BUFFER, this->mHandle);
		glad_glDisableVertexAttribArray(slot);
		glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	template<typename t>
	unsigned int Attribute<t>::Count() {
		return this->mCount;
	}

	template<typename T>
	unsigned int Attribute<T>::GetHandle() {
		return this->mHandle;
	}

}
