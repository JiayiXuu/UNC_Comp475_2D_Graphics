/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GMatrix_DEFINED
#define GMatrix_DEFINED

#include "GColor.h"
#include "GMath.h"
#include "GPoint.h"
#include "GRect.h"

class GMatrix {
public:
    GMatrix(const GMatrix& other) = default;

    /** Set the elements of the matrix.
     *
     *  [ a  b  c ]
     *  [ d  e  f ]
     *  [ 0  0  1 ]
     */
    GMatrix(float a, float b, float c, float d, float e, float f) {
        fMat[0] = a;    fMat[1] = b;    fMat[2] = c;
        fMat[3] = d;    fMat[4] = e;    fMat[5] = f;
    }

    /** Enums naming the 6 elements
     *
     *  [ SX  KX  TX ]
     *  [ KY  SY  TY ]
     *  [  0   0   1 ]
     *
     *  'S' - Scale
     *  'K' - sKew
     *  'T' - Translate
     */
    enum {
        SX, KX, TX,
        KY, SY, TY,
    };
    float operator[](int index) const {
        assert(index >= 0 && index < 6);
        return fMat[index];
    }
    float& operator[](int index) {
        assert(index >= 0 && index < 6);
        return fMat[index];
    }

    bool operator==(const GMatrix& m) {
        for (int i = 0; i < 6; ++i) {
            if (fMat[i] != m.fMat[i]) {
                return false;
            }
        }
        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // These methods must be implemented by the student.

    GMatrix();  // initialize to identity
    static GMatrix Translate(float tx, float ty);
    static GMatrix Scale(float sx, float sy);
    static GMatrix Rotate(float radians);

    /**
     *  Return the product of two matrices: a * b
     */
    static GMatrix Concat(const GMatrix& a, const GMatrix& b);

    /*
     *  Compute the inverse of this matrix, and store it in the "inverse" parameter, being
     *  careful to handle the case where 'inverse' might alias this matrix.
     *
     *  If this matrix is invertible, return true. If not, return false, and ignore the
     *  'inverse' parameter.
     */
    bool invert(GMatrix* inverse) const;

    /**
     *  Transform the set of points in src, storing the resulting points in dst, by applying this
     *  matrix. It is the caller's responsibility to allocate dst to be at least as large as src.
     *
     *  [ a  b  c ] [ x ]     x' = ax + by + c
     *  [ d  e  f ] [ y ]     y' = dx + ey + f
     *  [ 0  0  1 ] [ 1 ]
     *
     *  Note: It is legal for src and dst to point to the same memory (however, they may not
     *  partially overlap). Thus the following is supported.
     *
     *  GPoint pts[] = { ... };
     *  matrix.mapPoints(pts, pts, count);
     */
    void mapPoints(GPoint dst[], const GPoint src[], int count) const;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // These helper methods are implemented in terms of the previous methods.

    friend GMatrix operator*(const GMatrix& a, const GMatrix& b) {
        return Concat(a, b);
    }

    void preConcat(const GMatrix& m) {
        *this = Concat(*this, m);
    }

    void mapPoints(GPoint pts[], int count) const {
        this->mapPoints(pts, pts, count);
    }

    GPoint operator*(GPoint p) const {
        this->mapPoints(&p, 1);
        return p;
    }

private:
    float fMat[6];
};

#endif
