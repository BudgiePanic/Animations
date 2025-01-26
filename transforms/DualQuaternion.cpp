#include "DualQuaternion.h"
#include <Math.h>

namespace transforms {

    DualQuaternion operator+(const DualQuaternion& left, const DualQuaternion& right) {
        return DualQuaternion(left.real + right.real, left.dual + right.dual);
    }

    DualQuaternion operator*(const DualQuaternion& dualQuat, float scale) {
        return DualQuaternion(dualQuat.real * scale, dualQuat.dual * scale);
    }

    DualQuaternion operator*(const DualQuaternion& left, const DualQuaternion& right) {
        auto _left = normalized(left);
        auto _right = normalized(right);
        return DualQuaternion(_left.real * _right.real, (_left.real * _right.dual) + (_left.dual * _right.real));
    }
    
    bool operator==(const DualQuaternion& left, const DualQuaternion& right) {
        return left.real == right.real && left.dual == right.dual;
    }

    bool operator!=(const DualQuaternion& left, const DualQuaternion& right) {
        return !(left == right);
    }

    float dot(const DualQuaternion& left, const DualQuaternion& right) {
        return rotation::dot(left.real, right.real);
    }

    DualQuaternion conjugate(const DualQuaternion& dualQuat) {
        return DualQuaternion(rotation::conjugate(dualQuat.real), rotation::conjugate(dualQuat.dual));
    }

    constexpr float epsilon = 0.00001f;

    DualQuaternion normalized(const DualQuaternion& dualQuat) {
        float squared = rotation::dot(dualQuat.real, dualQuat.real);
        if (squared < epsilon) {
            return DualQuaternion();
        }
        float invMag = 1.0f / sqrtf(squared);
        return DualQuaternion((dualQuat.real * invMag), (dualQuat.dual * invMag));
    }

    void normalize(DualQuaternion& dualQuat) {
        float squared = rotation::dot(dualQuat.real, dualQuat.real);
        if (squared < epsilon) { return; }
        float invMag = 1.0f / sqrtf(squared);
        dualQuat.real = dualQuat.real * invMag;
        dualQuat.dual = dualQuat.dual * invMag;
    }

    DualQuaternion toDualQuaternion(const srt& transform) {
        rotation::quaternion position(
            transform.position.x,
            transform.position.y,
            transform.position.z,
            0
        );
        rotation::quaternion rotation = transform.rotation;
        position = rotation * position * 0.5f;
        return DualQuaternion(rotation, position);
    }

    srt toSRT(const DualQuaternion& dualQuat) {
        srt result;
        result.rotation = dualQuat.real;
        rotation::quaternion position = conjugate(dualQuat.real) * (dualQuat.dual * 2.0f);
        result.position = f3(position.x, position.y, position.z);
        return result;
    }

    f3 applyPoint(const DualQuaternion& dualQuat, const f3& point) {
        rotation::quaternion translation = conjugate(dualQuat.real) * (dualQuat.dual * 2.0f);
        return dualQuat.real * point + f3(translation.x, translation.y, translation.z);
    }

    f3 applyVector(const DualQuaternion& dualQuat, const f3& vector) {
        return dualQuat.real * vector;
    }

}
