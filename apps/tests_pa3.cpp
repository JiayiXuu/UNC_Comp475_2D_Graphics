/**
 *  Copyright 2018 Mike Reed
 */

#include "GMatrix.h"
#include "tests.h"

static bool ie_eq(float a, float b) {
    return fabs(a - b) <= 0.00001f;
}

static bool is_eq(const GMatrix& m, float a, float b, float c, float d, float e, float f) {
    //    printf("%g %g %g    %g %g %g\n", m[0], m[1], m[2], m[3], m[4], m[5]);
    return ie_eq(m[0], a) && ie_eq(m[1], b) && ie_eq(m[2], c) &&
    ie_eq(m[3], d) && ie_eq(m[4], e) && ie_eq(m[5], f);
}

static void test_matrix(GTestStats* stats) {
    GMatrix m;
    EXPECT_TRUE(stats, is_eq(m, 1, 0, 0, 0, 1, 0));
    m = GMatrix::Translate(2.5, -4);
    EXPECT_TRUE(stats, is_eq(m, 1, 0, 2.5, 0, 1, -4));
    m = GMatrix::Scale(2.5, -4);
    EXPECT_TRUE(stats, is_eq(m, 2.5, 0, 0, 0, -4, 0));
    m = GMatrix::Rotate(M_PI);
    EXPECT_TRUE(stats, is_eq(m, -1, 0, 0, 0, -1, 0));

    GMatrix m2, m3;
    m = GMatrix::Scale(2, 3);
    m2 = GMatrix::Scale(-1, -2);
    m3 = m * m2;
    EXPECT_TRUE(stats, is_eq(m3, -2, 0, 0, 0, -6, 0));
    m2 = GMatrix::Translate(5, 6);
    m3 = m2 * m;
    EXPECT_TRUE(stats, is_eq(m3, 2, 0, 5, 0, 3, 6));
}

static void test_matrix_inv(GTestStats* stats) {
    GMatrix m, m2, m3;

    EXPECT_TRUE(stats, m.invert(&m2) && m == m2);
    m = GMatrix::Scale(0.5f, 4);
    EXPECT_TRUE(stats, m.invert(&m2) && is_eq(m2, 2, 0, 0, 0, 0.25f, 0));
    // check for alias detection of src and dst
    EXPECT_TRUE(stats, m.invert(&m)  && is_eq(m,  2, 0, 0, 0, 0.25f, 0));

    m = GMatrix::Translate(3, 4) * GMatrix::Rotate(M_PI/3);
    EXPECT_TRUE(stats, m.invert(&m2));
    m3 = m * m2;
    EXPECT_TRUE(stats, is_eq(m3,  1, 0, 0, 0, 1, 0));
}

static void test_matrix_map(GTestStats* stats) {
    const GPoint src[] = { {0, 0}, {1, 1}, {-3, 4}, {0.5f, -0.125} };
    GPoint dst[4], dst2[4];

    GMatrix m = GMatrix::Translate(3, 4) * GMatrix::Scale(2, 2);
    m.mapPoints(dst, src, 4);
    const GPoint expected[] = { {3, 4}, {5, 6}, {-3, 12}, {4, 3.75f} };
    bool equal = true;
    for (int i = 0; i < 4; ++i) {
        equal &= dst[i].x() == expected[i].x();
        equal &= dst[i].y() == expected[i].y();
    }
    EXPECT_TRUE(stats, equal);

    m = GMatrix::Translate(3, 4) * GMatrix::Rotate(M_PI/3);
    m.mapPoints(dst, src, 4);
    memcpy(dst2, src, sizeof(src));
    m.mapPoints(dst2, dst2, 4);
    EXPECT_TRUE(stats, memcmp(dst, dst2, sizeof(src)) == 0);
}
