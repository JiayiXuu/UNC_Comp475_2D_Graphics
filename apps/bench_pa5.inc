/**
 *  Copyright 2020 Mike Reed
 */

#include "GPath.h"

class PathBench : public GBenchmark {
    const GISize    fSize;
    const float     fScale;
    const char*     fName;

    GPath           fPath;
    GPaint          fPaint;

public:
    PathBench(GISize size, float scale, const char* name) : fSize(size), fScale(scale), fName(name) {
        GRandom rand;
        auto rand_pt = [&]() {
            float x = rand.nextF() * fScale;
            float y = rand.nextF() * fScale;
            return GPoint{x, y};
        };

        fPath.moveTo(0, 0);
        for (int i = 0; i < 100; ++i) {
            auto p0 = rand_pt();
            fPath.lineTo(p0);
            auto p1 = rand_pt();
            auto p2 = rand_pt();
            fPath.quadTo(p1, p2);
            auto p3 = rand_pt();
            auto p4 = rand_pt();
            auto p5 = rand_pt();
            fPath.cubicTo(p3, p4, p5);
        }
    }

    const char* name() const override { return fName; }
    GISize size() const override { return fSize; }
    void draw(GCanvas* canvas) override {
        const int N = 10;
        canvas->clear({0,0,0,0});
        for (int i = 0; i < N; ++i) {
            canvas->drawPath(fPath, fPaint);
        }
    }
};
