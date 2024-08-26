#include "quaternion.h"
#include "../FloatHelp.h"
#include <cmath>

// Need more knowledge? >>> https://eater.net/quaternions

namespace rotation {

    constexpr float quaternionEpsilon = 0.000001f;
    const f3 worldRight = f3(1,0,0);
    const f3 worldUp = f3(0,1,0);
    const f3 worldForward = f3(0,0,1);

    quaternion angleAxis(float angle, const f3& axis)
    {
        const f3 norma = normalized(axis);
        const float ang = angle * 0.5f; // the rotation period of a quaternion is 2PI, so we need to half the value
        float s = sinf(ang); // scale the magnitude of the rotation axis down so the quaternion magnitude is still 1.
        return rotation::quaternion(
            norma.x * s, 
            norma.y * s,
            norma.z * s,
            cosf(ang)
        );
    }

    quaternion fromTo(const f3& from, const f3& to)
    {
        const f3 fromNorm = normalized(from), toNorm = normalized(to);
        if (fromNorm == toNorm) {
            // no rotation needed, identity quaternion provided
            return quaternion();
        }
        else if (fromNorm == (-1.0f * toNorm)) {
            // need a 180 degree rotation about an orthogonal axis
            // find basis axis vector that is 'most' orthogonal to the 'from' vector
            bool isYortho = fabsf(fromNorm.y) < fabsf(fromNorm.x);
            bool isZortho = fabsf(fromNorm.z) < fabsf(fromNorm.y) && fabsf(fromNorm.z) < fabsf(fromNorm.x);
            f3 orthogonalAxis = isZortho ? f3(0,0,1) : (isYortho ? f3(0,1,0) : f3(1,0,0));
            f3 rotationAxis = normalized(cross(fromNorm, orthogonalAxis));
            // w component is zero because cos(0.5 * PI) == 0 (PI rotation == 180 degree rotation)
            return quaternion(rotationAxis.x, rotationAxis.y, rotationAxis.z, 0.0f);
        }
        const f3 halfRotation = normalized(fromNorm + toNorm);
        const f3 rotationAxis = cross(fromNorm, halfRotation);
        float arcLen = dot(fromNorm, halfRotation); // cos(0.5 * rotation angle) == dot(fromVec, halfRotationVec)
        return rotation::quaternion(rotationAxis.x, rotationAxis.y, rotationAxis.z, arcLen);
    }

    f3 getAxis(const quaternion& quat)
    {
        return normalized(f3(quat.x, quat.y, quat.z));
    }

    float getAngle(const quaternion& quat)
    {
        return 2.0f * acosf(quat.w);
    }
    quaternion operator+(const quaternion& a, const quaternion& b)
    {
        return quaternion(
            a.x + b.x,
            a.y + b.y,
            a.z + b.z,
            a.w + b.w
        );
    }
    quaternion operator-(const quaternion& a, const quaternion& b)
    {
        return quaternion(
            a.x - b.x,
            a.y - b.y,
            a.z - b.z,
            a.w - b.w
        );
    }
    quaternion operator-(const quaternion& a)
    {
        return quaternion(-a.x, -a.y, -a.z, -a.w);
    }
    quaternion operator*(const quaternion& quat, float scalar)
    {
        return quaternion(quat.x * scalar, quat.y * scalar, quat.z * scalar, quat.w * scalar);
    }
    quaternion operator*(const quaternion& a, const quaternion& b)
    {
        // https://youtu.be/d4EgbgTm0Bg?si=P6N_6gdJDCbcVBkp&t=1138
        return quaternion(
             (b.x*a.w) + (b.y*a.z) - (b.z*a.y) + (b.w*a.x),
            -(b.x*a.z) + (b.y*a.w) + (b.z*a.x) + (b.w*a.y),
             (b.x*a.y) - (b.y*a.x) + (b.z*a.w) + (b.w*a.z),
            -(b.x*a.x) - (b.y*a.y) - (b.z*a.z) + (b.w*a.w)
        );
    }
    bool operator==(const quaternion& a, const quaternion& b)
    {
        return (
            fabsf(a.x - b.x) <= quaternionEpsilon &&
            fabsf(a.y - b.y) <= quaternionEpsilon &&
            fabsf(a.z - b.z) <= quaternionEpsilon &&
            fabsf(a.w - b.w) <= quaternionEpsilon
        );
    }
    bool operator!=(const quaternion& a, const quaternion& b)
    {
        return !(a==b);
    }
    bool sameOrientation(const quaternion& a, const quaternion& b)
    {
        return (
            (fabsf(a.x - b.x) <= quaternionEpsilon &&
             fabsf(a.y - b.y) <= quaternionEpsilon &&
             fabsf(a.z - b.z) <= quaternionEpsilon &&
             fabsf(a.w - b.w) <= quaternionEpsilon) || 
            (fabsf(a.x + b.x) <= quaternionEpsilon &&
             fabsf(a.y + b.y) <= quaternionEpsilon &&
             fabsf(a.z + b.z) <= quaternionEpsilon &&
             fabsf(a.w + b.w) <= quaternionEpsilon)
        );
    }
    float dot(const quaternion& a, const quaternion& b)
    {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }
    float lengthSquared(const quaternion& quat)
    {
        return (quat.x * quat.x) + (quat.y * quat.y) + (quat.z * quat.z) + (quat.w * quat.w);
    }
    float length(const quaternion& quat)
    {
        float lengthSq = lengthSquared(quat);
        if (lengthSq < quaternionEpsilon) {
            return 0.0f;
        }
        return sqrtf(lengthSq);
    }
    void normalize(quaternion& quat)
    {
        float lengthSq = lengthSquared(quat);
        if (lengthSq < quaternionEpsilon) {
            return;
        }
        float inverseLength = 1.0f / sqrtf(lengthSq);
        quat.x *= inverseLength;
        quat.y *= inverseLength;
        quat.z *= inverseLength;
        quat.w *= inverseLength;
    }
    quaternion normalized(const quaternion& quat)
    {
        float lengthSq = lengthSquared(quat);
        if (lengthSq < quaternionEpsilon) {
            return quaternion();
        }
        float inverseLength = 1.0f / sqrtf(lengthSq);
        return quaternion(
            quat.x * inverseLength,
            quat.y * inverseLength,
            quat.z * inverseLength,
            quat.w * inverseLength
        );
    }
    quaternion conjugate(const quaternion& quat)
    {
        return quaternion(
            -quat.x,
            -quat.y,
            -quat.z,
            quat.w
        );
    }
    quaternion inverse(const quaternion& quat)
    {
        // conjugate divided by the squared length of the quaternion
        float lengthSq = lengthSquared(quat);
        if (lengthSq < quaternionEpsilon) {
            return quaternion();
        }
        float reciprocal = 1.0f / lengthSq;
        return quaternion(
            -quat.x * reciprocal,
            -quat.y * reciprocal,
            -quat.z * reciprocal,
            quat.w * reciprocal
        );
    }
    bool isNormalized(const quaternion& quat)
    {
        return FloatCompare(lengthSquared(quat), 1.0f) == AequalsB;
    }
    bool isPure(const quaternion& quat)
    {
        return FloatCompare(quat.w, 0.0f) && isNormalized(quat);
    }
    /// <summary>
    /// Transforms a vector by the transform encoded in the quaternion.
    /// If the quaternion is a unit quaternion the transformation will result in a rotation and no displacement.
    /// </summary>
    /// <param name="rotation"></param>
    /// <param name="vector"></param>
    /// <returns></returns>
    f3 operator*(const quaternion& rotation, const f3& vector)
    {
        // To transform the vector we would multiply the vector by the quaternion
        // Then we would multiply that product by the inverse of the quaternion
        // The resulting quaternion's "rotation axis" will be the final rotated vector
        // r = q * v * q^-1
        // The book author asserts that the formula for this operation can be simplified to the below code
        return 
            rotation.vector * 2.0f * dot(rotation.vector, vector) +
            vector * (rotation.scalar * rotation.scalar - dot(rotation.vector, rotation.vector)) +
            cross(rotation.vector, vector) * 2.0f * rotation.scalar;
    }
    /// <summary>
    /// The "LERP" function. It is called mix because the quaternion rotation is along an arc, not a straight line.
    /// Assumes the two quaternions are in the desired neighborhood.
    /// </summary>
    /// <param name="from"></param>
    /// <param name="to"></param>
    /// <param name="t"></param>
    /// <returns></returns>
    quaternion mix(const quaternion& from, const quaternion& to, float t)
    {
        return from * (1.0f - t) + to * t;
    }
    /// <summary>
    /// Assumes the two quaternions are in the desired neighborhood.
    /// Fast approximation of slerp.
    /// </summary>
    /// <param name="from"></param>
    /// <param name="to"></param>
    /// <param name="t"></param>
    /// <returns></returns>
    quaternion nlerp(const quaternion& from, const quaternion& to, float t)
    {
        return normalized(from + (to - from) * t);
    }
    /// <summary>
    /// Causes consistent rotation velocity. Expensive.
    /// Creates a quaternion representing a rotation 't'% along the way from 'from' to 'to' called delta
    /// and concatinates delta to the from quaternion.
    /// </summary>
    /// <param name="from"></param>
    /// <param name="to"></param>
    /// <param name="t"></param>
    /// <returns></returns>
    quaternion slerp(const quaternion& from, const quaternion& to, float t)
    {
        // fall back to nlerp if 'from' and 'to' are close together (dot product == 1 means they're the same)
        float difference = fabsf(dot(from, to));
        if (difference > (1.0f - quaternionEpsilon)) {
            return nlerp(from, to, t);
        }
        quaternion delta = (inverse(from) * to)^t;
        return normalized(delta * from);
    }
    /// <summary>
    /// Adjusts the rotation angle encoded in a quaternion. i.e. power of 0.5 will reduce the rotation angle by half.
    /// </summary>
    /// <param name="quat"></param>
    /// <param name="power"></param>
    /// <returns></returns>
    quaternion operator^(const quaternion& quat, float power)
    {
        float rotationAngle = getAngle(quat);
        f3 rotationAxis = normalized(quat.vector);
        // to ensure the result quaternion is normalized we need to scale back the quation amount and rotation axis
        float angle = cosf(power * rotationAngle * 0.5f);
        float axisScale = sinf(power * rotationAngle * 0.5f);
        return quaternion(
            rotationAxis.x * axisScale,
            rotationAxis.y * axisScale,
            rotationAxis.z * axisScale,
            angle
        );
    }
    /// <summary>
    /// Create a rotation that will 'look' in a direction
    /// </summary>
    /// <param name="direction"></param>
    /// <param name="up"></param>
    /// <returns></returns>
    quaternion lookRotation(const f3& direction, const f3& up)
    {
        // get the quaternion that rotates the world forward vector to the desired direction
        f3 objectForward = normalized(direction);
        quaternion worldToObjectForward = fromTo(worldForward, objectForward);
        // correct the rotation quaternion so that the desired 'up' vector is respected
        f3 desiredUp = normalized(up);
        f3 objectRight = cross(desiredUp, objectForward);
        desiredUp = cross(objectForward, objectRight);
        f3 objectUp = worldToObjectForward * worldUp;
        quaternion objectToDesiredUp = fromTo(objectUp, desiredUp);
        quaternion result = worldToObjectForward * objectToDesiredUp;
        return normalized(result);
    }

    // === (extracting rotation matrices from transform matrices and converting to quaternions) and (quaternions to rotation matrices) ===

    mat4f toMatrix(const quaternion& quat)
    {
        f3 right = quat * worldRight;
        f3 up = quat * worldUp;
        f3 forward = quat * worldForward;
        // remember, column major memory layout of matrix
        return mat4f(
            right.x,   right.y,   right.z,   0,
            up.x,      up.y,      up.z,      0,
            forward.x, forward.y, forward.z, 0,
            0,         0,         0,         1
        );
    }

    quaternion toQuaternion(const mat4f& matrix)
    {
        f3 up = normalized(f3(matrix.c1.x, matrix.c1.y, matrix.c1.z));
        f3 forward = normalized(f3(matrix.c2.x, matrix.c2.y, matrix.c2.z));
        f3 right = cross(up, forward);
        up = cross(forward, right);
        return lookRotation(forward, up);
    }

}


