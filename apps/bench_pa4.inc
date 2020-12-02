/**
 *  Copyright 2019 Mike Reed
 */

class GradientBench : public ShaderBench {
public:
    GradientBench(const GColor colors[], int count, const char* name) : ShaderBench(name, 20) {
        fShader = GCreateLinearGradient({0, 0}, GPoint{W, H}, colors, count);
    }
};
