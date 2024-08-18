#include "Mat3f.h"
#include "Mat2f.h"

float determinant(const Mat3f& m)
{
    float answer = 0.0f;
    answer += m.r0c0 * cofactor(m, 0, 0);
    answer += m.r0c1 * cofactor(m, 0, 1);
    answer += m.r0c2 * cofactor(m, 0, 2);
    return answer;
}

Mat2::Mat2f submatrix(const Mat3f& m, int row, int col)
{
    float temp[2][2] = { {0.0f, 0.0f},{0.0f, 0.0f} };
    int rIndex = 0, cIndex = 0; // the column and row of the temp array we are writing into 
    // write the submatrix that excludes row and col into the temp array
    for (int r = 0; r < 3; r++) {
        if (r == row) { continue; }
        for (int c = 0; c < 3; c++) {
            if (c == col) { continue; }
            int index = r * 3 + c;
            temp[rIndex][cIndex++] = m.v[index];
            if (cIndex == 3) {
                cIndex = 0;
                rIndex++;
            }
        }
    }
    return Mat2::Mat2f(temp[0][0], temp[0][1], temp[1][0], temp[1][1]);
}

float minor(const Mat3f& m, int row, int col)
{
    Mat2::Mat2f submat = submatrix(m, row, col);
    return Mat2::determinant(submat);
}

float cofactor(const Mat3f& m, int row, int col)
{
    float answer = minor(m,row,col);
    if ((row + col) % 2 != 0) {
        answer *= -1.0f;
    }
    return answer;
}
