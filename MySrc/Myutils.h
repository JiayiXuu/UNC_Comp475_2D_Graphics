#include <vector>

class MyColor {
public:
    int   fA;
    int   fR;
    int   fG;
    int   fB;

    static MyColor MakeARGB255(int a, int r, int g, int b) {
        MyColor c = { a, r, g, b };
        return c;
    }

};

static unsigned int Div255Shift(int a){
    return (a*257+128*257) >> 16;
}

static GColor Premul(GColor color){
    return color.MakeARGB(color.fA,color.fA*color.fR,color.fA*color.fG,color.fA*color.fB);
}

static MyColor Scale255(GColor color){
    return MyColor::MakeARGB255(GRoundToInt(color.fA*255.f), GRoundToInt(color.fR*255.f), GRoundToInt(color.fG*255.f), GRoundToInt(color.fB*255.f));
}

static GPixel MakeGPixel(GColor color){
    MyColor s_premul = Scale255(Premul(color));
    return GPixel_PackARGB(s_premul.fA, s_premul.fR, s_premul.fG, s_premul.fB);
}
