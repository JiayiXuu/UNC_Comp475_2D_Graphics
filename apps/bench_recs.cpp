/**
 *  Copyright 2015 Mike Reed
 */

#include "bench.h"
#include "GCanvas.h"
#include "GBitmap.h"
#include "GColor.h"
#include "GRandom.h"
#include "GRect.h"
#include <string>

static GColor rand_color(GRandom& rand, bool forceOpaque = false) {
    GColor c { rand.nextF(), rand.nextF(), rand.nextF(), rand.nextF() };
    if (forceOpaque) {
        c.fA = 1;
    }
    return c;
}

static GRect rand_rect(GRandom& rand, const GRect& bounds) {
    const float x = bounds.width();
    const float y = bounds.height();
    float tmp[4] {
        rand.nextF() * x - bounds.left(), rand.nextF() * y - bounds.top(),
        rand.nextF() * x - bounds.left(), rand.nextF() * y - bounds.top()
    };
    return GRect::MakeXYWH(std::min(tmp[0], tmp[2]), std::min(tmp[1], tmp[3]),
                           std::max(tmp[0], tmp[2]), std::max(tmp[1], tmp[3]));
}

class RectsBench : public GBenchmark {
    enum { W = 200, H = 200 };
    const bool fForceOpaque;
public:
    RectsBench(bool forceOpaque) : fForceOpaque(forceOpaque) {}

    const char* name() const override { return fForceOpaque ? "rects_opaque" : "rects_blend"; }
    GISize size() const override { return { W, H }; }
    void draw(GCanvas* canvas) override {
        const int N = 500;
        const GRect bounds = GRect::MakeLTRB(-10, -10, W + 10, H + 10);
        GRandom rand;
        for (int i = 0; i < N; ++i) {
            GColor color = rand_color(rand, fForceOpaque);
            GRect rect = rand_rect(rand, bounds);
            canvas->fillRect(rect, color);
        }
    }
};

class SingleRectBench : public GBenchmark {
    const GISize    fSize;
    const GRect     fRect;
    const char*     fName;
public:
    SingleRectBench(GISize size, GRect r, const char* name) : fSize(size), fRect(r), fName(name) {}

    const char* name() const override { return fName; }
    GISize size() const override { return fSize; }
    void draw(GCanvas* canvas) override {
        const int N = 10000;
        GRandom rand;
        for (int i = 0; i < N; ++i) {
            GColor color = rand_color(rand);
            canvas->fillRect(fRect, color);
        }
    }
};

#include "bench_pa2.inc"
#include "bench_pa3.inc"
#include "bench_pa4.inc"
#include "bench_pa5.inc"
#include "bench_pa6.inc"

const GBenchmark::Factory gBenchFactories[] {
    []() -> GBenchmark* { return new RectsBench(false); },
    []() -> GBenchmark* { return new RectsBench(true);  },
    []() -> GBenchmark* {
        return new SingleRectBench({2,2}, GRect::MakeLTRB(-1000, -1000, 1002, 1002), "rect_big");
    },
    []() -> GBenchmark* {
        return new SingleRectBench({1000,1000}, GRect::MakeLTRB(500, 500, 502, 502), "rect_tiny");
    },

    // pa2
    []() -> GBenchmark* { return new PolyRectsBench(false); },
    []() -> GBenchmark* { return new PolyRectsBench(true);  },
    []() -> GBenchmark* { return new CirclesBench(false); },
    []() -> GBenchmark* { return new CirclesBench(true);  },
    []() -> GBenchmark* { return new ModesBench({0.0, 1, 0.5, 0.25}, "modes_0"); },
    []() -> GBenchmark* { return new ModesBench({0.5, 1, 0.5, 0.25}, "modes_half"); },
    []() -> GBenchmark* { return new ModesBench({1.0, 1, 0.5, 0.25}, "modes_1"); },

    // pa3
    []() -> GBenchmark* { return new BitmapBench("apps/spock.png", "bitmap_opaque"); },
    []() -> GBenchmark* { return new BitmapBench("apps/oldwell.png", "bitmap_alpha"); },

    // pa4
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1, 1, 0, 0 }, { 0, 0, 1, 1 }};
        return new GradientBench(colors, 2, "gradient_2");
    },
    []() -> GBenchmark* {
        const GColor colors[] = {{ 1, 1, 0, 0 }, { 1, 0, 1, 1 }, {0.5, 0, 1, 0}};
        return new GradientBench(colors, 3, "gradient_3");
    },

    // pa5
    []() -> GBenchmark* {
        return new PathBench({256, 256}, 256, "path_unclipped");
    },
    []() -> GBenchmark* {
        return new PathBench({256, 256}, 1024, "path_clipped");
    },

    // pa6
    []() -> GBenchmark* {
        const GPoint verts[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
        const GColor colors[] = {{ 1,1,0,0 }, { 1,0,1,0 }, {1,0,0,1}, {1,1,1,1}};
        const int indices[] = { 0, 1, 2,  2, 3, 0 };
        return new MeshBench(verts, colors, nullptr, 2, indices, "mesh_colors");
    },
    []() -> GBenchmark* {
        const GPoint verts[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
        const int indices[] = { 0, 1, 2,  2, 3, 0 };
        return new MeshBench(verts, nullptr, verts, 2, indices, "mesh_texs");
    },
    []() -> GBenchmark* {
        const GPoint verts[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
        const GColor colors[] = {{ 1,1,0,0 }, { 1,0,1,0 }, {1,0,0,1}, {1,1,1,1}};
        const int indices[] = { 0, 1, 2,  2, 3, 0 };
        return new MeshBench(verts, colors, verts, 2, indices, "mesh_both");
    },

    nullptr,
};
