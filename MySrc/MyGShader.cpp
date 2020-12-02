#include "GShader.h"
#include "GBitmap.h"
#include "GMatrix.h"
#include "Mytile.h"

class MyGShader : public GShader
{
public:
    MyGShader(const GBitmap &m_bitmap, const GMatrix &localMatrix, GShader::TileMode tile) : m_BitMap(m_bitmap), m_matix(localMatrix)
    {
        inv_h = 1.f / m_bitmap.height();
        inv_w = 1.f / m_bitmap.width();
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

    // Return true iff all of the GPixels that may be returned by this shader will be opaque.
    bool isOpaque()
    {
        return m_BitMap.isOpaque();
    }

    // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
    bool setContext(const GMatrix &ctm)
    {
        if (!ctm.invert(&inverse))
        {
            return false;
        }
        (ctm * m_matix).invert(&inverse);
        inverse = GMatrix(inv_w, 0, 0, 0, inv_h, 0) * inverse;
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
        GPoint loc = inverse * GPoint{x + 0.5f, y + 0.5f};
        for (int i = 0; i < count; ++i)
        {
            float x = t_func(loc.fX) * m_BitMap.width();
            float y = t_func(loc.fY) * m_BitMap.height();
            row[i] = *m_BitMap.getAddr(x, y);
            loc.fX += dx;
            loc.fY += dy;
        }
        return;
    }

private:
    GBitmap m_BitMap;
    GMatrix m_matix;
    GMatrix inverse;
    TileFunction t_func;
    float inv_w;
    float inv_h;
    float dx;
    float dy;
};

/**
 *  Return a subclass of GShader that draws the specified bitmap and the local matrix.
 *  Returns null if the either parameter is invalid.
 */
std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap &m_bitmap, const GMatrix &localMatrix, GShader::TileMode tile)
{
    if (!m_bitmap.pixels())
    {
        return nullptr;
    }
    return std::unique_ptr<GShader>(new MyGShader(m_bitmap, localMatrix, tile));
}
