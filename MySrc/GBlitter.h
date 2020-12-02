#include "GPixel.h"
#include "GShader.h"
#include "MyBlendMode.h"
#include "GBitmap.h"
#include "GPixel.h"

typedef void (*ScanFunction)(int xl, int xr, int y, GShader* shader, const GBitmap fDevice, BlendFunc b_func, GPixel src);

void ShadeSpan(int xl, int xr, int y, GShader* shader, const GBitmap fDevice, BlendFunc b_func,GPixel src)
{
    int size = xr - xl;
    GPixel rows[size];
    GPixel *row = rows;
    shader->shadeRow(xl, y, size, row);
    for (int x = xl; x < xr; x++)
    {
        GPixel *dst = fDevice.getAddr(x, y);
        *dst = b_func(row[x - xl], *dst);
    }
}

void Span(int xl, int xr, int y, GShader* shader, const GBitmap fDevice, BlendFunc b_func,GPixel src)
{
    // xl = std::max(0,std::min(fDevice.width()-1,xl));
    // xr = std::max(0,std::min(fDevice.height()-1,xr));
    for (int x = xl; x < xr; x++)
    {
        GPixel *dst = fDevice.getAddr(x, y);
        *dst = b_func(src, *dst);
    }
}

void SpanTri(int xl, int xr, int y, GShader* shader, const GBitmap fDevice)
{
    int size = xr - xl;
    GPixel rows[size];
    GPixel *row = rows;
    shader->shadeRow(xl, y, size, row);
    for (int x = xl; x < xr; x++)
    {
        GPixel *dst = fDevice.getAddr(x, y);
        *dst = row[x - xl];
    }
}