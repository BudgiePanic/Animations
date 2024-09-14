#pragma once

// animation namespace: data structures and functions used for animation data
namespace anim {
	
	enum class Interpolate {
		Constant,	// Step wise interpolation between keyframes 
		Linear,		// Linear interpolation between keyframes
		Cubic		// Hermite spline interpolation between keyframe values. See curve.h for details
	};

}
