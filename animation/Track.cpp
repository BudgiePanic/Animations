#include "Track.h"

namespace anim {

	template Track<float, 1>;
	template Track<f3, 3>;
	template Track<rotation::quaternion, 4>;

	namespace trackHelpers {
		inline float Interpolate(float a, float b, float t) {
			return a + ((b - a) * t);
		}
		inline f3 Interpolate(const f3& a, const f3& b, float t) {
			return lerp(a,b,t);
		}
		inline rotation::quaternion Interpolate(const rotation::quaternion& a, const rotation::quaternion& b, float t) {
			// check quaternion neighborhood (see quaternion.h for details)
			if (rotation::dot(a,b) < 0.0f) {
				return rotation::normalized(rotation::mix(a,-b,t));
			}
			return rotation::normalized(rotation::mix(a, b, t));
		}
		// Not sure why the author wanted to code it this way
		// If a different type of frame is added, this nonsense is gonna be duplicated for each new type
		// You'd think this would be put into each templated verion's functions
		// re-validates an interpolated quaternion
		inline float HermitePass(float f) { return f; }
		inline f3 HermitePass(const f3& vec) { return vec; }
		inline rotation::quaternion HermitePass(const rotation::quaternion& q) {
			return rotation::normalized(q);
		}
		inline void NeighborhoodPass(const float& a, float& b) { return; }
		inline void NeighborhoodPass(const f3& a, f3& b) { return; }
		inline void NeighborhoodPass(const rotation::quaternion& a, rotation::quaternion& b) {
			if (rotation::dot(a, b) < 0.0f) {
				b = -b;
			}
		}
	}
	// TODO implement track functions

}
