#include "GShader.h"
#include "GBitmap.h"
#include "GMatrix.h"
#include "Myutils.h"
class GMyComposeShader : public GShader
{
public:
    GMyComposeShader(GShader* Shader0, GShader* Shader1)
    {
        color_shader = Shader0;
        bitmap_shader = Shader1;
    }

    // Return true iff all of the GPixels that may be returned by this shader will be opaque.
    bool isOpaque()
    {
        return color_shader->isOpaque() && bitmap_shader->isOpaque();
    }

    // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
    bool setContext(const GMatrix &ctm)
    {
        return color_shader->setContext(ctm) && bitmap_shader->setContext(ctm);
    }

    /**
     *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
     *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
     *  can hold at least [count] entries.
     */
    void shadeRow(int x, int y, int count, GPixel row[])
    {
        GPixel arow[count];
        GPixel brow[count];
        color_shader->shadeRow(x, y, count, arow);
        bitmap_shader->shadeRow(x, y, count, brow);
        for (int i = 0; i < count; i++)
        {
            row[i] = GPixel_PackARGB(
                Div255Shift(GPixel_GetA(arow[i]) * GPixel_GetA(brow[i])),
                Div255Shift(GPixel_GetR(arow[i]) * GPixel_GetR(brow[i])),
                Div255Shift(GPixel_GetG(arow[i]) * GPixel_GetG(brow[i])),
                Div255Shift(GPixel_GetB(arow[i]) * GPixel_GetB(brow[i]))
            );
        }
    }

private:
    GShader* color_shader;
    GShader* bitmap_shader;
};


GShader* GCreateComposeShader(GShader* Shader0, GShader* Shader1)
{
    return (new GMyComposeShader(Shader0,Shader1));
}
