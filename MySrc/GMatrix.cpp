#include "GMatrix.h"
#include "GPoint.h"

GMatrix::GMatrix()
{
    fMat[0] = 1;
    fMat[1] = 0;
    fMat[2] = 0;
    fMat[3] = 0;
    fMat[4] = 1;
    fMat[5] = 0;
}

GMatrix GMatrix::Translate(float tx, float ty)
{
    return GMatrix(1, 0, tx, 0, 1, ty);
}

GMatrix GMatrix::Scale(float sx, float sy)
{
    return GMatrix(sx, 0, 0, 0, sy, 0);
}

GMatrix GMatrix::Rotate(float radians)
{
    return GMatrix(cos(radians), -sin(radians), 0, sin(radians), cos(radians), 0);
}

GMatrix GMatrix::Concat(const GMatrix &a, const GMatrix &b)
{
    return GMatrix(
        a[GMatrix::SX] * b[GMatrix::SX] + a[GMatrix::KX] * b[GMatrix::KY],
        a[GMatrix::SX] * b[GMatrix::KX] + a[GMatrix::KX] * b[GMatrix::SY],
        a[GMatrix::SX] * b[GMatrix::TX] + a[GMatrix::KX] * b[GMatrix::TY] + a[GMatrix::TX],
        a[GMatrix::KY] * b[GMatrix::SX] + a[GMatrix::SY] * b[GMatrix::KY],
        a[GMatrix::KY] * b[GMatrix::KX] + a[GMatrix::SY] * b[GMatrix::SY],
        a[GMatrix::KY] * b[GMatrix::TX] + a[GMatrix::SY] * b[GMatrix::TY] + a[GMatrix::TY]);
}

/*
     *  If this matrix is invertible, return true and (if not null) set the inverse parameter.
     *  If this matrix is not invertible, return false and ignore the inverse parameter.
     */

bool GMatrix::invert(GMatrix *inverse) const
{
    float det = fMat[GMatrix::SX] * fMat[GMatrix::SY] - fMat[GMatrix::KX] * fMat[GMatrix::KY];
    if (det == 0)
    {
        return false;
    }
    else
    {
        float inv_det = 1 / det;
        GMatrix inv_m = GMatrix(
            fMat[GMatrix::SY] * inv_det,
            -fMat[GMatrix::KX] * inv_det,
            (fMat[GMatrix::KX] * fMat[GMatrix::TY] - fMat[GMatrix::TX] * fMat[GMatrix::SY]) * inv_det,
            -fMat[GMatrix::KY] * inv_det,
            fMat[GMatrix::SX] * inv_det,
            (fMat[GMatrix::TX] * fMat[GMatrix::KY] - fMat[GMatrix::SX] * fMat[GMatrix::TY]) * inv_det);


        *inverse = inv_m;
        return true;
    }
}

/**
     *  Transform the set of points in src, storing the resulting points in dst, by applying this
     *  matrix. It is the caller's responsibility to allocate dst to be at least as large as src.
     *
     *  Note: It is legal for src and dst to point to the same memory (however, they may not
     *  partially overlap). Thus the following is supported.
     *
     *  GPoint pts[] = { ... };
     *  matrix.mapPoints(pts, pts, count);
     */
void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const
{
    for (int i = 0; i < count; i++)
    {
        dst[i] = GPoint::Make(src[i].fX * fMat[GMatrix::SX] + src[i].fY * fMat[GMatrix::KX] + fMat[GMatrix::TX],
                              src[i].fX * fMat[GMatrix::KY] + src[i].fY * fMat[GMatrix::SY] + fMat[GMatrix::TY]);
    }
}
