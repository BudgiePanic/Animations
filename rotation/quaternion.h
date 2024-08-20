#ifndef _H_QUAT_
#define _H_QUAT_

#include "../Vector3.h"
#include "../Mat4f.h"

struct quaternion {
	union
	{
		struct {
			float x; // i
			float y; // j
			float z; // k 
			float w; // _
		};
		struct {
			f3 vector;
			float scalar;
		};
		float v[4];
	};
	inline quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	inline quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

#endif