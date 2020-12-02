/**
 *  Copyright 2018 Mike Reed
 */

#include "GPath.h"
#include "GPoint.h"
#include "tests.h"

static void test_path(GTestStats* stats) {
    GPoint pts[2];
    GPath path;

    EXPECT_EQ(stats, path.bounds(), GRect::MakeWH(0,0));
    EXPECT_EQ(stats, GPath::Iter(path).next(pts), GPath::kDone);

    path.moveTo(10, 20);
    EXPECT_EQ(stats, path.bounds(), GRect::MakeXYWH(10, 20, 0, 0));
    path.lineTo(30, 0);
    EXPECT_EQ(stats, path.bounds(), GRect::MakeLTRB(10, 0, 30, 20));

    GPath::Iter iter(path);
    EXPECT_EQ(stats, iter.next(pts), GPath::kMove);
    EXPECT_EQ(stats, pts[0], (GPoint{10, 20}));
    EXPECT_EQ(stats, iter.next(pts), GPath::kLine);
    EXPECT_EQ(stats, pts[0], (GPoint{10, 20}));
    EXPECT_EQ(stats, pts[1], (GPoint{30, 0}));
    EXPECT_EQ(stats, iter.next(pts), GPath::kDone);

    GPath p2 = path;
    EXPECT_EQ(stats, p2.bounds(), path.bounds());
    p2.transform(GMatrix::Scale(0.5, 2));
    EXPECT_EQ(stats, p2.bounds(), GRect::MakeLTRB(5, 0, 15, 40));

    const GPoint p[] = { {10, 10}, {20, 20}, {30, 30} };
    int N = 0;
    path.reset();
    path.moveTo(p[0]).lineTo(p[1]).lineTo(p[2]);    N++;
    path.moveTo(p[0]).lineTo(p[1]).lineTo(p[2]);    N++;
    path.moveTo(p[0]).lineTo(p[1]).lineTo(p[2]);    N++;

    GPath::Edger edger(path);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < 3; ++j) {
            assert(edger.next(pts) == GPath::kLine);
            assert(pts[0] == p[j]);
            assert(pts[1] == p[(j+1)%3]);
        }
    }
    assert(edger.next(pts) == GPath::kDone);
}

static bool expect_iter(const GPath& path, const GPoint expected_pts[], int count) {
    GPath::Iter iter(path);
    GPoint pts[2];
    if (count > 0) {
        if (iter.next(pts) != GPath::kMove || pts[0] != expected_pts[0]) {
            return false;
        }
        for (int i = 1; i < count; ++i) {
            if (iter.next(pts) != GPath::kLine || pts[1] != expected_pts[i]) {
                return false;
            }
        }
    }
    return iter.next(pts) == GPath::kDone;
}

static void test_path_rect(GTestStats* stats) {
    GPath p;
    EXPECT_TRUE(stats, GRect::MakeWH(0, 0) == p.bounds());

    GRect r = GRect::MakeLTRB(10, 20, 30, 40);
    p.addRect(r, GPath::kCW_Direction);
    EXPECT_TRUE(stats, p.bounds() == r);
    const GPoint pts0[] = {{10, 20}, {30, 20}, {30, 40}, {10, 40}};
    EXPECT_TRUE(stats, expect_iter(p, pts0, 4));

    p.reset();
    p.addRect(r, GPath::kCCW_Direction);
    EXPECT_TRUE(stats, p.bounds() == r);
    const GPoint pts1[] = {{10, 20}, {10, 40}, {30, 40}, {30, 20}};
    EXPECT_TRUE(stats, expect_iter(p, pts1, 4));
}

static void test_path_poly(GTestStats* stats) {
    const GPoint pts[] = {{-10, -10}, {10, 0}, {0, 10}};
    GPath p;
    p.addPolygon(pts, GARRAY_COUNT(pts));
    EXPECT_TRUE(stats, GRect::MakeLTRB(-10, -10, 10, 10) == p.bounds());
    EXPECT_TRUE(stats, expect_iter(p, pts, GARRAY_COUNT(pts)));
}

static void test_path_transform(GTestStats* stats) {
    GPath p;
    const GPoint pts[] = {{10, 20}, {30, 40}, {50, 60}, {70, 80}};
    GRect r = GRect::MakeLTRB(10, 20, 70, 80);

    p.addPolygon(pts, GARRAY_COUNT(pts));
    EXPECT_TRUE(stats, r == p.bounds());

    p.transform(GMatrix::Translate(-30, 20));
    r.offset(-30, 20);
    EXPECT_TRUE(stats, r == p.bounds());
}
