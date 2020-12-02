#include "GShader.h"
#include "GBitmap.h"
#include "GMatrix.h"

class GMyProxyShader : public GShader
{
public:
    GMyProxyShader(GShader *Shader, GPoint p0, GPoint p1, GPoint p2, GPoint s0, GPoint s1, GPoint s2)
    {
        realShader = Shader;
        GVector v1 = p1 - p0;
        GVector v2 = p2 - p0;
        local_matrix_P = GMatrix(v1.fX, v2.fX, p0.fX, v1.fY, v2.fY, p0.fY);
        GVector S1 = s1-s0;
        GVector S2 = s2-s0;
        GMatrix s = GMatrix(S1.fX, S2.fX, s0.fX, S1.fY, S2.fY, s0.fY);
        s.invert(&local_matrix_S);
    }

    // Return true iff all of the GPixels that may be returned by this shader will be opaque.
    bool isOpaque()
    {
        return realShader->isOpaque();
    }

    // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
    bool setContext(const GMatrix &ctm)
    {
        return realShader->setContext(ctm*local_matrix_P*local_matrix_S);
    }

    /**
     *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
     *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
     *  can hold at least [count] entries.
     */
    void shadeRow(int x, int y, int count, GPixel row[])
    {
        realShader->shadeRow(x,y,count,row);
        return;
    }

private:
    GShader *realShader;
    GMatrix local_matrix_P;
    GMatrix local_matrix_S;
};


GShader* GCreatProxyShader(GShader *Shader, GPoint p0, GPoint p1, GPoint p2, GPoint s0, GPoint s1, GPoint s2)
{
    return (new GMyProxyShader(Shader, p0, p1, p2, s0, s1, s2));
}
