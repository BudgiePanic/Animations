#pragma once

// animation relates data structures and functions
namespace anim {
	
	enum class Interpolate {
		Constant,	// Step wise interpolation between keyframes 
		Linear,		// Linear interpolation between keyframes
		Cubic		// Hermite spline interpolation between keyframe values. See curve.h for details
	};

}
