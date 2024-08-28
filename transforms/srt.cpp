#include "srt.h"
#include "../FloatHelp.h"

namespace transforms {

// the "inverse" of zero scale is just zero
#define inverseScale(scale) fabs(scale) < epsilon ? 0.0f : (1.0f / scale)

    const f3 xBasis = f3(1, 0, 0);
    const f3 yBasis = f3(0, 1, 0);
    const f3 zBasis = f3(0, 0, 1);

    srt combine(const srt& a, const srt& b)
    {
        srt result;
        result.scale = a.scale * b.scale;
        result.rotation = b.rotation * a.rotation;
        // scale, rotate, translate the sub component position
        result.position = a.rotation * (a.scale * b.position);
        result.position = a.position + result.position;
        return result;
    }
    srt inverse(const srt& transform)
    {
        srt result;
        result.rotation = inverse(transform.rotation);
        result.scale.x = inverseScale(transform.scale.x);
        result.scale.y = inverseScale(transform.scale.y);
        result.scale.z = inverseScale(transform.scale.z);
        f3 translationInverse = transform.position * -1.0f;
        result.position = result.rotation * (result.scale * translationInverse);
        return result;
    }
    srt mix(const srt& from, const srt& to, float t)
    {
        // check the rotation quaternion neighborhooding (see quaternion.h for details)
        rotation::quaternion toRotation = to.rotation; // make copy of toRotation
        if (dot(from.rotation, toRotation) < 0.0f) {
            toRotation = -toRotation;
        }
        return srt(
            lerp(from.position, to.position, t),
            nlerp(from.rotation, toRotation, t),
            lerp(from.scale, to.scale, t)
        );
    }
    mat4f toMatrix(const srt& transform)
    {
        // basis vectors encode how the coordinate space xyz unit vectors are affected by the transform
        f3 x = transform.rotation * xBasis;
        f3 y = transform.rotation * yBasis;
        f3 z = transform.rotation * zBasis;
        x = x * transform.scale.x;
        y = y * transform.scale.y;
        z = z * transform.scale.z;
        const f3& translation = transform.position;
        return mat4f(
            x.x, x.y, x.z, 0,
            y.x, y.y, y.z, 0,
            z.x, z.y, z.z, 0,
            translation.x, translation.y, translation.z, 1.0f
        );
    }
    srt toSRT(const mat4f& transform)
    {
        srt result;
        result.position = f3(transform.r0c3, transform.r1c3, transform.r2c3);
        result.rotation = rotation::toQuaternion(transform);
        // remove the translation column from the SRT matrix and then multiply the resulting SR matrix by inverse of R
        // This leaves us with the (scale + skew) matrix. We extract the scale from this approximation matrix.
        // There are expensive techniques that identify the matrix skew and remove it, leaving just scale, but this 
        // approximation is faster: https://research.cs.wisc.edu/graphics/Courses/838-s2002/Papers/polar-decomp.pdf
        mat4f rotationScaleSkew(
            transform.v[0], transform.v[1], transform.v[2], 0.0f,
            transform.v[4], transform.v[5], transform.v[6], 0.0f,
            transform.v[7], transform.v[8], transform.v[9], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        mat4f rotationInverse = rotation::toMatrix(inverse(result.rotation));
        mat4f scaleSkew = rotationScaleSkew * rotationInverse;
        // scale is encoded in (r0c0 -> 0, r1c1 -> 5, r2c2 -> 10)
        result.scale = f3(scaleSkew.v[0], scaleSkew.v[5], scaleSkew.v[10]);
        return result;
    }
    f3 applyPoint(const srt& transform, const f3& point)
    {
        return transform.position + (transform.rotation * (transform.scale * point));
    }
    f3 applyVector(const srt& transform, const f3& vector)
    {
        return transform.rotation * (transform.scale * vector);
    }
}

