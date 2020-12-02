/**
 *  Copyright 2019 Mike Reed
 */

#include "GShader.h"

class ShaderBench : public GBenchmark {
protected:
    enum { W = 200, H = 200 };
    const char* fName;
    const int fLoops;
    std::unique_ptr<GShader> fShader;

    ShaderBench(const char* name, int loops) : fName(name), fLoops(loops) {}

public:
    const char* name() const override { return fName; }
    GISize size() const override { return { W, H }; }

    void draw(GCanvas* canvas) override {
        GPaint paint(fShader.get());
        for (int i = 0; i < fLoops; ++i) {
            canvas->drawPaint(paint);
        }
    }
};

class BitmapBench : public ShaderBench {
public:
    BitmapBench(const char imagePath[], const char* name) : ShaderBench(name, 50) {
        GBitmap bm;
        bm.readFromFile(imagePath);
//        printf("%s is opaque:%d\n", imagePath, bm.isOpaque());
        GMatrix mx = GMatrix::Scale(W * 1.0 / bm.width(), H * 1.0 / bm.height());
        fShader = GCreateBitmapShader(bm, mx);
    }
};

