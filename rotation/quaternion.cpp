#include "quaternion.h"
#include "../FloatHelp.h"

namespace rotation {

    quaternion angleAxis(float angle, const f3& axis)
    {
        return rotation::quaternion();
    }

    quaternion fromTo(const f3& from, const f3& to)
    {
        return rotation::quaternion();
    }

    f3 getAxis(const quaternion& quat)
    {
        return f3();
    }

    float getAngle(const quaternion& quat)
    {
        return 0.0f;
    }
    quaternion operator+(const quaternion& a, const quaternion& b)
    {
        return quaternion();
    }
    quaternion operator-(const quaternion& a, const quaternion& b)
    {
        return quaternion();
    }
    quaternion operator-(const quaternion& a)
    {
        return quaternion();
    }
    quaternion operator*(const quaternion& quat, float scalar)
    {
        return quaternion();
    }
    quaternion operator*(const quaternion& a, const quaternion& b)
    {
        return quaternion();
    }
    bool operator==(const quaternion& a, const quaternion& b)
    {
        return false;
    }
    bool operator!=(const quaternion& a, const quaternion& b)
    {
        return false;
    }
    bool sameOrientation(const quaternion& a, const quaternion& b)
    {
        return false;
    }
    float dot(const quaternion& a, const quaternion& b)
    {
        return 0.0f;
    }
    float lengthSquared(const quaternion& quat)
    {
        return 0.0f;
    }
    float length(const quaternion& quat)
    {
        return 0.0f;
    }
    void normalize(quaternion& quat)
    {
    }
    quaternion normalized(const quaternion& quat)
    {
        return quaternion();
    }
    quaternion conjugate(const quaternion& quat)
    {
        return quaternion();
    }
    quaternion inverse(const quaternion& quat)
    {
        // conjugate divided by the squared length of the quaternion
        return quaternion();
    }
    bool isNormalized(const quaternion& quat)
    {
        return FloatCompare(lengthSquared(quat), 1.0f) == AequalsB;
    }
    bool isPure(const quaternion& quat)
    {
        return FloatCompare(quat.w, 0.0f) && isNormalized(quat);
    }
    quaternion mix(const quaternion& from, const quaternion& to, float t)
    {
        return quaternion();
    }

    quaternion nlerp(const quaternion& from, const quaternion& to, float t)
    {
        return quaternion();
    }

    quaternion slerp(const quaternion& from, const quaternion& to, float t)
    {
        return quaternion();
    }

    quaternion operator^(const quaternion& quat, float power)
    {
        return quaternion();
    }

    quaternion lookRotation(const f3& direction, const f3& up)
    {
        return quaternion();
    }

    mat4f toMatrix(const quaternion& quat)
    {
        return mat4f();
    }

    quaternion toQuaternion(const mat4f& matrix)
    {
        return quaternion();
    }

}


