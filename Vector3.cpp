#include "Vector3.h"
#include "FloatHelp.h"

f3 operator+(const f3& a, const f3& b)
{
    return f3(
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    );
}

f3 operator-(const f3& a, const f3& b)
{
    return f3(
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    );
}

f3 operator*(const f3& a, const float s)
{
    return f3(
        a.x * s,
        a.y * s,
        a.z * s
    );
}

f3 operator*(const f3& a, const f3& b)
{
    return f3(
        a.x * b.x,
        a.y * b.y,
        a.z * b.z
    );
}

f3 operator/(const f3& a, const f3& b)
{
    return f3(
        a.x / b.x,
        a.y / b.y,
        a.z / b.z
    );
}

bool operator==(const f3& a, const f3& b) 
{
    return
        FloatCompare(a.x, b.x) == AequalsB &&
        FloatCompare(a.y, b.y) == AequalsB &&
        FloatCompare(a.z, b.z) == AequalsB;
}

bool operator!=(const f3& a, const f3& b)
{
    return !(a == b);
}

inline float dot(const f3& a, const f3& b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline float lengthSquared(const f3& a) 
{
    return (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
}

/// <summary>
/// The distance between two vectors can be found by length(a - b);
/// </summary>
/// <param name="a"></param>
/// <returns></returns>
float length(const f3& a)
{
    float length = lengthSquared(a);
    if (length < epsilon) { return 0.0f; }
    return sqrtf(length);
}

void normalize(f3& a) 
{
    float lengthSq = lengthSquared(a);
    if (lengthSq < epsilon) {
        return; // can't normalize vector of magnitude zero
    }
    float inv = 1.0f / sqrtf(lengthSq);
    a.x *= inv;
    a.y *= inv;
    a.z *= inv;
}

f3 normalized(const f3& a)
{
    float lengthSq = lengthSquared(a);
    if (lengthSq < epsilon) {
        return a; // can't normalize vector of magnitude zero
    }
    float inv = 1.0f / sqrtf(lengthSq);
    return f3(
        a.x * inv,
        a.y * inv,
        a.z * inv
    );
}

/// <summary>
/// theta = inv-cos((A dot B) / |A|*|B|)
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns>The angle between a & b in radians</returns>
float angle(const f3& a, const f3& b) 
{
    float lengthA = lengthSquared(a);
    float lengthB = lengthSquared(b);
    if (lengthA < epsilon || lengthB < epsilon) {
        return 0.0f; // can't get angle from 0 magnitude vector
    }
    float length = sqrtf(lengthA) * sqrtf(lengthB);
    return acosf(dot(a, b) / length);
}

inline f3 project(const f3& a, const f3& b) 
{
    float lengthB = length(b); 
    if (lengthB < epsilon) { return f3(); }
    return b * (dot(a, b) / lengthB);
}

f3 reject(const f3& a, const f3& b)
{
    return a - project(a, b);
}

f3 reflect(const f3& a, const f3& b)
{
    //                              ^
    // a 'bounce' type reflection \/
    float lengthB = length(b);
    if (lengthB < epsilon) { return f3(); }
    float scale = 2.0f * (dot(a, b) / lengthB);
    f3 projection = b * scale;
    return a - projection;
}

/// <summary>
/// |A cross B| = |A| * |B| * sin(angle(A, B))
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
inline f3 cross(const f3& a, const f3& b)
{
    return f3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}
