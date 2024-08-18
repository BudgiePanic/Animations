#include "Mat2f.h"

float Mat2::determinant(const Mat2f& mat)
{
    return (mat.r0c0 * mat.r1c1) - (mat.r0c1 * mat.r1c0);
}
