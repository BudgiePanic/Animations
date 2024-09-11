#include "Mat4f.h"
#include "FloatHelp.h"
#include "Mat3f.h"
#include <iostream>

// |a,b,c,d|   |00,01,02,03|   
// |e,f,g,h| * |04,05,06,07| => result(rY,cX) = dot(a[rYc0, rYc1, rYc2, rYc3],b[r0cX,r1cX,r2cX,r3cX])
// |i,j,k,l|   |08,09,10,11| => result(r1,c2) = dot([e,f,g,h],[02,06,10,14])
// |m,n,o,p|   |12,13,14,15|
#define MATRIX_DOT(aRow, bCol) a.v[0 * 4 + aRow] * b.v[bCol * 4 + 0] + a.v[1 * 4 + aRow] * b.v[bCol * 4 + 1] + a.v[2 * 4 + aRow] * b.v[bCol * 4 + 2] + a.v[3 * 4 + aRow] * b.v[bCol * 4 + 3]

bool operator==(const mat4f& a, const mat4f& b)
{
    for (int i = 0; i < 16; i++) {
        if (FloatCompare(a.v[i], b.v[i]) != AequalsB) {
            return false;
        }
    }
    return true;
}

bool operator!=(const mat4f & a, const mat4f & b)
{
    return !(a == b);
}

mat4f operator+(const mat4f& a, const mat4f& b)
{
    return mat4f(
        (a.r0c0 + b.r0c0), (a.r1c0 + b.r1c0), (a.r2c0 + b.r2c0), (a.r3c0 + b.r3c0),
        (a.r0c1 + b.r0c1), (a.r1c1 + b.r1c1), (a.r2c1 + b.r2c1), (a.r3c1 + b.r3c1),
        (a.r0c2 + b.r0c2), (a.r1c2 + b.r1c2), (a.r2c2 + b.r2c2), (a.r3c2 + b.r3c2),
        (a.r0c3 + b.r0c3), (a.r1c3 + b.r1c3), (a.r2c3 + b.r2c3), (a.r3c3 + b.r3c3)
    );
}

mat4f operator*(const mat4f& a, float f)
{
    return mat4f(
        (a.r0c0 * f), (a.r1c0 * f), (a.r2c0 * f), (a.r3c0 * f),
        (a.r0c1 * f), (a.r1c1 * f), (a.r2c1 * f), (a.r3c1 * f),
        (a.r0c2 * f), (a.r1c2 * f), (a.r2c2 * f), (a.r3c2 * f),
        (a.r0c3 * f), (a.r1c3 * f), (a.r2c3 * f), (a.r3c3 * f)
    );
}

mat4f operator*(const mat4f& a, const mat4f& b)
{
    return mat4f(
        MATRIX_DOT(0,0), MATRIX_DOT(1,0), MATRIX_DOT(2,0), MATRIX_DOT(3,0),
        MATRIX_DOT(0,1), MATRIX_DOT(1,1), MATRIX_DOT(2,1), MATRIX_DOT(3,1),
        MATRIX_DOT(0,2), MATRIX_DOT(1,2), MATRIX_DOT(2,2), MATRIX_DOT(3,2),
        MATRIX_DOT(0,3), MATRIX_DOT(1,3), MATRIX_DOT(2,3), MATRIX_DOT(3,3)
    );
}

f4 operator*(const mat4f& a, const f4& b)
{
    const float x = (b.x * a.r0c0) + (b.y * a.r0c1) + (b.z * a.r0c2) + (b.w * a.r0c3);
    const float y = (b.x * a.r1c0) + (b.y * a.r1c1) + (b.z * a.r1c2) + (b.w * a.r1c3);
    const float z = (b.x * a.r2c0) + (b.y * a.r2c1) + (b.z * a.r2c2) + (b.w * a.r2c3);
    const float w = (b.x * a.r3c0) + (b.y * a.r3c1) + (b.z * a.r3c2) + (b.w * a.r3c3);
    return f4(x,y,z,w);
}

f3 multiplyVector(const mat4f& matrix, const f3& vector)
{
    const float x = (vector.x * matrix.r0c0) + (vector.y * matrix.r0c1) + (vector.z * matrix.r0c2);
    const float y = (vector.x * matrix.r1c0) + (vector.y * matrix.r1c1) + (vector.z * matrix.r1c2);
    const float z = (vector.x * matrix.r2c0) + (vector.y * matrix.r2c1) + (vector.z * matrix.r2c2);
    return f3(x,y,z);
}

f3 multiplyPoint(const mat4f& matrix, const f3& point)
{
    const float x = (point.x * matrix.r0c0) + (point.y * matrix.r0c1) + (point.z * matrix.r0c2) + (1.0f * matrix.r0c3);
    const float y = (point.x * matrix.r1c0) + (point.y * matrix.r1c1) + (point.z * matrix.r1c2) + (1.0f * matrix.r1c3);
    const float z = (point.x * matrix.r2c0) + (point.y * matrix.r2c1) + (point.z * matrix.r2c2) + (1.0f * matrix.r2c3);
    return f3(x,y,z);
}

f3 multiply(const mat4f& matrix, const f3& tuple, float& w)
{
    const float x = (tuple.x * matrix.r0c0) + (tuple.y * matrix.r0c1) + (tuple.z * matrix.r0c2) + (w * matrix.r0c3);
    const float y = (tuple.x * matrix.r1c0) + (tuple.y * matrix.r1c1) + (tuple.z * matrix.r1c2) + (w * matrix.r1c3);
    const float z = (tuple.x * matrix.r2c0) + (tuple.y * matrix.r2c1) + (tuple.z * matrix.r2c2) + (w * matrix.r2c3);
    return f3(x,y,z);
}

// b => e, c => i, g => j, d => m, h => n, l => o   
// A E I M 
// B F J N 
// C G K O  
// D H L P
// [A]   [B]   [C]   [D]    | [E]   [F]   [G]   [H]    | [I]   [J]   [K]   [L]    | [M]   [N]   [O]   [P]
// [0]   [1]   [2]   [3]    | [4]   [5]   [6]   [7]    | [8]   [9]   [10]  [11]   | [12]  [13]  [14]  [15]
/// <summary>
/// flips the rows and columns of the matrix
/// </summary>
/// <param name="m"></param>
void transpose(mat4f& m)
{
    float b = m.v[1], c = m.v[2], g = m.v[6], d = m.v[3], h = m.v[7], l = m.v[11];
    m.v[1] = m.v[4]; m.v[4] = b;
    m.v[2] = m.v[8]; m.v[8] = c;
    m.v[6] = m.v[9]; m.v[9] = g;
    m.v[3] = m.v[12]; m.v[12] = d;
    m.v[7] = m.v[13]; m.v[13] = h;
    m.v[11] = m.v[14]; m.v[14] = l;
}

mat4f transposed(const mat4f& m)
{
    return mat4f(
        m.xx, m.yx, m.zx, m.tx,
        m.xy, m.yy, m.zy, m.ty,
        m.xz, m.yz, m.zz, m.tz,
        m.xw, m.yw, m.zw, m.tw
    );
}

float determinant(const mat4f& m)
{
    float answer = 0.0f;
    answer += m.r0c0 * cofactor(m, 0, 0);
    answer += m.r0c1 * cofactor(m, 0, 1);
    answer += m.r0c2 * cofactor(m, 0, 2);
    answer += m.r0c3 * cofactor(m, 0, 3);
    return answer;
}

mat4f inverse(const mat4f& m)
{
    float det = determinant(m);
    if (FloatCompare(det, 0.0f) == AequalsB) {
        // if the determinant is zero, then the inverse does not exist
        std::cout << "cannot invert a matrix, returning the identity matrix instead";
        return mat4f();
    }
    mat4f invert = mat4f();
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float factor = cofactor(m, row, col);
            int index = row * 4 + col;
            invert.v[index] = factor / det;
        }
    }
    return invert;
}

float cofactor(const mat4f& m, int row, int col) {
    float min = minor(m, row, col);
    if ((row + col) % 2 != 0) {
        min *= -1.0f;
    }
    return min;
}

float minor(const mat4f& m, int row, int col)
{
    return determinant(submatrix(m, row, col));
}

Mat3f submatrix(const mat4f& m, int row, int col)
{
    float temp[3][3] = { {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
    int _row = 0, _col = 0;
    for (int r = 0; r < 4; r++) {
        if (r == row) { continue; }
        for (int c = 0; c < 4; c++) {
            if (c == col) { continue; }
            int index = (c * 4) + r;
            const float value = m.v[index];
            temp[_row][_col++] = value;
            if (_col == 3) {
                _row++;
                _col = 0;
            }
        }
    }
    return Mat3f(
        temp[0][0], temp[1][0], temp[2][0],
        temp[0][1], temp[1][1], temp[2][1],
        temp[0][2], temp[1][2], temp[2][2]
    );
}

void invert(mat4f& m)
{
    float det = determinant(m);
    if (!(FloatCompare(det, 0.0f) == AequalsB)) {
        // if the determinant is not zero, then the inverse does not exist
        std::cout << "cannot invert matrix, converting matrix to identity instead";
        m = mat4f();
        return;
    }
    mat4f invert = mat4f();
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            float factor = cofactor(m, row, col);
            int index = row * 4 + col;
            invert.v[index] = factor / det;
        }
    }
    m = invert;
    return;
}

mat4f perspective(float fov, float aspect, float near, float far)
{
    float ymax = near * tanf(fov * 3.14159265359f / 360.0f);
    float xmax = ymax * aspect;
    return frustum(-xmax, xmax, -ymax, ymax, near, far);
}

mat4f orthographic(float left, float right, float bottom, float top, float near, float far)
{
    if (FloatCompare(left, right) == AequalsB ||
        FloatCompare(top, bottom) == AequalsB ||
        FloatCompare(near, far) == AequalsB)
    {
        std::cout << "invalid orthographic parameters, returning identity instead\n";
        return mat4f();
    }
    return mat4f(
        ((2.0f)/(right-left)), 0, 0, 0,
        0,((2.0f)/(top-bottom)), 0, 0,
        0,0,((-2.0f)/(far-near)), 0,
        (-((right+1.0f)/(right-1.0f))), (-((top+bottom)/(top-bottom))), (-((far+near)/(far-near))), 1.0f
    );
}

mat4f lookAt(const f3& position, const f3& target, const f3& up)
{
    f3 forward = normalized(target - position) * -1.0f;
    f3 right = cross(up, forward); // right handed coordinate system
    if (right == f3(0.0f,0.0f,0.0f)) { return mat4f(); }
    normalize(right);
    f3 u = normalized(cross(forward, right)); // "true up"
    f3 translate = f3(
        -dot(right, position),
        -dot(u, position),
        -dot(forward, position)
    );
    return mat4f(
        right.x, u.x, forward.x, 0.0f,
        right.y, u.y, forward.y, 0.0f,
        right.z, u.z, forward.z, 0.0f,
        translate.x, translate.y, translate.z, 1.0f
    );
}

/// <summary>
/// Creates a view frustum matrix
/// </summary>
/// <returns></returns>
mat4f frustum(float left, float right, float bottom, float top, float near, float far)
{
    if (FloatCompare(left, right) == AequalsB ||
        FloatCompare(top, bottom) == AequalsB || 
        FloatCompare(near, far) == AequalsB)
    {
        std::cout << "invalid frustum parameters, returning identity instead\n";
        return mat4f();
    }
    return mat4f(
        ((2.0f * near)/(right-left)), 0, 0, 0,
        0,((2.0f*near)/(top-bottom)), 0, 0,
        ((right+left)/(right-left)), ((top+bottom)/(top-bottom)), ((-(far+near))/(far-near)), -1,
        0,0,((-2.0f*far*near)/(far-near)), 0
    );
}
