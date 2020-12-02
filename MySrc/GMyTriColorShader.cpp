#include "GShader.h"
#include "GMatrix.h"
#include "MyTile.h"
#include "Myutils.h"
class GMyTriColorShader : public GShader
{
public:
    GMyTriColorShader(GPoint p0, GPoint p1, GPoint p2, GColor c0, GColor c1, GColor c2)
    {
        // my_colors = colors;
        GVector v1 = p1 - p0;
        GVector v2 = p2 - p0;
        local_matrix = GMatrix(v1.fX, v2.fX, p0.fX, v1.fY, v2.fY, p0.fY);
        color0 = c0.pinToUnit();
        color1 = c1.pinToUnit();
        color2 = c2.pinToUnit();
    }
    bool isOpaque()
    {
        if (color0.fA < 1.0f && color1.fA < 1.0f && color2.fA < 1.0f)
        {
            return false;
        }
        return true;
    }

    // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
    bool setContext(const GMatrix &ctm)
    {
        if (!ctm.invert(&inverse))
        {
            return false;
        }
        (ctm * local_matrix).invert(&inverse);
        dx = inverse[GMatrix::SX];
        dy = inverse[GMatrix::KY];
        return true;
    }

    /**
     *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
     *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
     *  can hold at least [count] entries.
     */
    void shadeRow(int x, int y, int count, GPixel row[])
    {

        // calculate the first point
        GPoint loc = inverse * GPoint{x + 0.5f, y + 0.5f};

        for (int i = 0; i < count; ++i)
        {
            float x = t_func(loc.fX);
            float y = t_func(loc.fY);

            float coef = 1 - x - y;
            GColor color = GColor::MakeARGB(
                coef * color0.fA + x * color1.fA + y * color2.fA,
                coef * color0.fR + x * color1.fR + y * color2.fR,
                coef * color0.fG + x * color1.fG + y * color2.fG,
                coef * color0.fB + x * color1.fB + y * color2.fB);

            // color.pinToUnit();
            row[i] = MakeGPixel(color);

            loc.fX += dx;
            loc.fY += dy;
        }
        return;
    }

private:
    GColor color0;
    GColor color1;
    GColor color2;
    GMatrix local_matrix;
    GMatrix inverse;
    TileFunction t_func = Clamp;
    float dx;
    float dy;
};

GShader *GCreatTriColorShader(GPoint p0, GPoint p1, GPoint p2, GColor c0, GColor c1, GColor c2)
{
    return (new GMyTriColorShader(p0, p1, p2, c0, c1, c2));
}
