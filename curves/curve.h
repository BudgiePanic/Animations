#pragma once

namespace curves {

	template<typename T>
	class Bezier {
	public:
		T point1;
		T control1;
		T point2;
		T control2;
	};

	template<typename T>
	inline T Interpolate(const Bezier<T>& curve, float t);
	// The following comments describe the process of simplifying the Bezier spline curve interpolation function
	/*
	   A spline has two points and two control points. 
		c1*  _  *c2
		  | / \ |
		  |/   \|
		p1*     *p2
	*/
	/*  T Lerp(T& a, T& b, float t) {
	        return (a * (1.0f - t)) + (b * t);
	    } 
	*/
	/*
	template<typename T>
	T Interpolate(Bezier<T>& curve, float t) {
	    // Naive implementation
		T p1c1 = Lerp(curve.point1, curve.control1, t);
		T p2c2 = Lerp(curve.point2, curve.control2, t);
		T c1c2 = Lerp(curve.control1, curve.control2, t);
		T p1c1_c1c2 = Lerp(p1c1, c1c2, t);
		T c1c2_p2c2 = Lerp(c1c2, p2c2, t);
		T value = Lerp(p1c1_c1c2, c1c2_p2c2);
		return value;
	}*/
	/*
	template<typename T>
	T Interpolate(const Bezier<T>& curve, float t) {
		constexpr float tMinus = 1.0f - t;
		// inline expand LERP calls
		const T p1c1 = (curve.point1 * tMinus) + (curve.control1 * t);
		const T p2c2 = (curve.point2 * tMinus) + (curve.control2 * t);
		const T c1c2 = (curve.control1 * tMinus) + (curve.control2 * t);
		const T p1c1_c1c2 = (p1c1 * tMinus) + (c1c2 * t);
		const T c1c2_p2c2 = (c1c2 * tMinus) + (p2c2 * t);
		const T value = (p1c1_c1c2 * tMinus) + (c1c2_p2c2 * t);
		return value;
	}
	*/
	/*
	template<typename T>
	T Interpolate(const Bezier<T>& curve, float t) {
		constexpr float tMinus = 1.0f - t;
		// inline LERP calls
		// ((P1 * (1 - t) + C1 * t) * (1 - t) + (C1 * (1 - t) + C2 * t) * t) * (1 - t) + ((C1 * (1 - t) + C2 * t) * (1 - t) + (C2 * (1 - t) + P2 * t) * t) * t
		return 
			((curve.point1 * tMinus + curve.control1 * t) * tMinus + (curve.control1 * tMinus + curve.control2 * t) * t) * tMinus + ((curve.control1 * tMinus + curve.control2 * t) * tMinus + (curve.control2 * tMinus + curve.point2 * t) * t) * t ;
	}
	*/
	/*
	template<typename T>
	T Interpolate(const Bezier<T>& curve, float t) {
		// According to the book author, the inline lerp expression can be expanded, and like terms grouped to produce this expression:
		// (-P1*t^3) + (3*P1*t^2) - (3*P1*t) + P1 + (3*C1*t^3) - (6*C1*t^2) + (3*C1*t) - (3*C2*t^3) + (3*C2*t^2) + (P2*t^3)
		constexpr float ttt = t * t * t, tt = t*t;
		return
			(-1.0f * curve.point1 * ttt) +
			(3.0f * curve.point1 * tt) -
			(3.0f * curve.point1 * t) +
			(curve.point1) +
			(3.0f * curve.control1 * ttt) -
			(6.0f * curve.control1 * tt) + 
			(3.0f * curve.control1 * t) - 
			(3.0f * curve.control2 * ttt) + 
			(3.0f * curve.control2 * tt) + 
			(curve.point2 * ttt);
	}
	*/
	/*
	template<typename T>
	T Interpolate(const Bezier<T>& curve, float t) {
		// the bezier curve terms can be isolated to further simplify the expression
		// [P1 * (-t^3 + 3*t^2 - 3*t + 1)] + [C1 * (3*t^3 - 6*t^2 + 3*t)] + [C2 * (-3*t^3 + 3*t^2)] + [P2 * (t^3)]
		constexpr float ttt = t * t * t, tt = t * t;
		return 
			curve.point1 * (-ttt + 3.0f*tt - 3.0f*t + 1.0f) + 
			curve.control1 * (3.0f*ttt - 6.0f*tt + 3.0f*t) + 
			curve.control2 * (-3.0f*ttt + 3.0f*tt) + 
			curve.point2 * (ttt);
	}
	*/

	template<typename T>
	T Interpolate(const Bezier<T>& curve, float t) {
		// If we further simplify the internal terms, we arrive at the following result
		// [P1 * ((1-t)^3)] + [C1 * (3*((1-t)^2)*t)] + [C2 * (3*(1-t)*t^2)] + [P2 * (t^3)]
		// The final results are the basis functions for cubic bezier spline curve.
		// The basis functions describe the influence of each curve parameter in the range [0..1]
		constexpr float oneMinus = 1.0f - t;
		return 
			curve.point1 * (oneMinus * oneMinus * oneMinus) + 
			curve.control1 * (3.0f * (oneMinus * oneMinus) * t) + 
			curve.control2 * (3.0f * oneMinus * (t * t)) + 
			curve.point2 * (t*t*t);
	}

	template<typename T>
	T Hermite(float t, T& point1, T& slope1, T& point2, T& slope2);

	template<typename T>
	T Hermite(float t, T& point1, T& slope1, T& point2, T& slope2)
	{
		// Now that we've seen how the basis functions are derived for the cubic bezier spline curve
		// we look at the hermite spline curve. The book author simply provides the basis functions for the 
		// curve, and does not explain how they were derived.
		// The control parameters for the hermite spline are:
		// - two points
		// - two tangents of the points on the spline curve (AKA slopes)
		constexpr float a = (1.0f + 2.0f * t) * ((1.0f - t) * (1.0f - t));
		constexpr float b = t * ((1.0f - t) * (1.0f - t));
		constexpr float c = (t * t) * (3.0f - 2.0f * t);
		constexpr float d = (t * t) * (t - 1.0f);
		return 
			point1 * (a) +
			slope1 * (b) +
			point2 * (c) +
			slope2 * (d);
	}

	/* 
		Lastly, the book author states that in 3D content authoring applications like Maya and Blender 
		Animation curves are commonly supplied as either hermite splines or cubic beizier curves
	*/
}
