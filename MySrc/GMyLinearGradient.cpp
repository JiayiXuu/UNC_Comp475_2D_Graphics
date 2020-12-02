#include "GShader.h"
#include "GMatrix.h"
#include "Myutils.h"
#include "Mytile.h"
#include "GMySingleColorShader.h"

class GMyLinearGradient : public GShader
{
public:
    GMyLinearGradient(GPoint p0, GPoint pn, const GColor colors[], int count, TileMode tile)
    {
        // my_colors = colors;
        float dx = pn.fX - p0.fX;
        float dy = pn.fY - p0.fY;
        local_matrix = GMatrix(dx, -dy, p0.fX, dy, dx, p0.fY);
        my_colors.resize(count);
        for (int i = 0; i < count; i++)
        {
            my_colors[i] = colors[i];
        }
        c_count = count;
        if (tile == TileMode::kClamp)
        {
            t_func = Clamp;
        }
        else if (tile == TileMode::kRepeat)
        {
            t_func = Repeat;
        }
        else if (tile == TileMode::kMirror)
        {
            t_func = Mirror;
        }
    }
    bool isOpaque()
    {
        for (int i = 0; i < c_count; i++)
        {
            if (my_colors[i].fA < 1.0f)
            {
                return false;
            }
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
            float t = loc.fX;
            t = t_func(t);
            t = t * (c_count - 1);
            int index = (int)t; //color index
            t = t - index;      //x location between ci and ci+1

            GColor c1 = my_colors[index].pinToUnit();
            GColor c2 = my_colors[index + 1].pinToUnit();

            GColor color = GColor::MakeARGB(
                c1.fA * (1 - t) + c2.fA * t,
                c1.fR * (1 - t) + c2.fR * t,
                c1.fG * (1 - t) + c2.fG * t,
                c1.fB * (1 - t) + c2.fB * t);

            color.pinToUnit();
            row[i] = MakeGPixel(color);

            loc.fX += dx;
        }
        return;
    }

private:
    int c_count;
    // const GColor *my_colors;
    std::vector<GColor> my_colors;
    GMatrix local_matrix;
    GMatrix inverse;
    TileFunction t_func;
    float dx;
    float dy;
};

std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GShader::TileMode tile)
{
    if (count < 1)
    {
        return nullptr;
    }
    if (count == 1)
    {
        return std::unique_ptr<GShader>(new GMySingleColorShader(colors));
    }
    return std::unique_ptr<GShader>(new GMyLinearGradient(p0, p1, colors, count, tile));
}
