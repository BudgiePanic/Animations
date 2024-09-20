#pragma once

namespace anim {

	/*
		Curves could have been implemented as being compositions of several scalar curves.
		- VectorCurve
		  - x:ScalarCurve
		  - y:ScalarCurve
		  - z:ScalarCurve
		>> Allows: each component of the curve to use a different interpolation method

		OR they can be implemented as tracks with explicit frame types:
		- VectorCurve
		  - xyz: VectorFrame[...]
		>> requires more memory because each curve component must have the same number of keyframes, even when its not necessary
	*/

	template<unsigned int DIMENSION>
	class Frame {
	public:
		float value[DIMENSION];
		float in[DIMENSION];	// incoming and 
		float out[DIMENSION];	// outgoing sample tangents for hermite spline interpolation
		float timestamp;
	};

	typedef Frame<1> FrameScalar;
	typedef Frame<3> FrameVector;
	typedef Frame<4> FrameQuaternion;

}
