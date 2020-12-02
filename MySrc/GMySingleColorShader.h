#include "GShader.h"
#include "GMatrix.h"

class GMySingleColorShader : public GShader
{
public:
    GMySingleColorShader(const GColor colors[])
    {
        pixel = MakeGPixel(colors[0]);
    }
    bool isOpaque()
    {
        if (GPixel_GetA(pixel) < 255)
        {
            return false;
        }
        return true;
    }

    // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
    bool setContext(const GMatrix &ctm)
    {
        return true;
    }

    /**
     *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
     *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
     *  can hold at least [count] entries.
     */
    void shadeRow(int x, int y, int count, GPixel row[])
    {

        for (int i = 0; i < count; ++i)
        {
            row[i] = pixel;
        }
        return;
    }

private:
    GPixel pixel;
};
