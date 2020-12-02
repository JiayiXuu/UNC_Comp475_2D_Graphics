#include "GMatrix.h"

#include "GPath.h"

/**
 *  Append a new contour, made up of the 4 points of the specified rect, in the specified
 *  direction. The contour will begin at the top-left corner of the rect.
 */
GPath &GPath::addRect(const GRect &rect, Direction dir)
{
    this->moveTo(GPoint::Make(rect.left(), rect.top()));

    if (dir == Direction::kCW_Direction)
    {
        this->lineTo(GPoint::Make(rect.right(), rect.top()));
        this->lineTo(GPoint::Make(rect.right(), rect.bottom()));
        this->lineTo(GPoint::Make(rect.left(), rect.bottom()));
    }
    else
    {
        this->lineTo(GPoint::Make(rect.left(), rect.bottom()));
        this->lineTo(GPoint::Make(rect.right(), rect.bottom()));
        this->lineTo(GPoint::Make(rect.right(), rect.top()));
    }

    return *this;
}

/**
 *  Append a new contour with the specified polygon. Calling this is equivalent to calling
 *  moveTo(pts[0]), lineTo(pts[1..count-1]).
 */
GPath &GPath::addPolygon(const GPoint pts[], int count)
{
    if (count < 2)
    {
        return *this;
    }
    else
    {
        this->moveTo(pts[0]);
        for (int i = 1; i < count; ++i)
        {
            this->lineTo(pts[i]);
        }
    }

    return *this;
}

/**
 *  Return the bounds of all of the control-points in the path.
 *
 *  If there are no points, return {0, 0, 0, 0}
 */
GRect GPath::bounds() const
{
    int count = this->fPts.size();
    if (count == 0)
    {
        return GRect::MakeLTRB(0, 0, 0, 0);
    }

    float l = fPts[0].fX;
    float r = fPts[0].fX;
    float t = fPts[0].fY;
    float b = fPts[0].fY;
    for (int i = 0; i < count; i++)
    {
        l = std::min(fPts[i].fX, l);
        t = std::min(fPts[i].fY, t);
        r = std::max(fPts[i].fX, r);
        b = std::max(fPts[i].fY, b);
    }
    return GRect::MakeLTRB(l, t, r, b);
}

/**
 *  Transform the path in-place by the specified matrix.
 */
void GPath::transform(const GMatrix &matrix)
{
    matrix.mapPoints(this->fPts.data(), this->fPts.data(), this->fPts.size());
}

GPath &GPath::addCircle(GPoint center, float radius, Direction dir)
{
    float h = tanf(3.1415926 / 8.0);
    float c = 1 / sqrt(2);
    h *= radius;
    c *= radius;
    float r = radius;
    GPoint points[16] = {{ r+center.x(),    center.y()},
                         { r+center.x(),  h+center.y()},
                         { c+center.x(),  c+center.y()},
                         { h+center.x(),  r+center.y()},
                         {   center.x(),  r+center.y()},
                         {-h+center.x(),  r+center.y()},
                         {-c+center.x(),  c+center.y()},
                         {-r+center.x(),  h+center.y()},
                         {-r+center.x(),   center.y()},
                         {-r+center.x(), -h+center.y()},
                         {-c+center.x(), -c+center.y()},
                         {-h+center.x(), -r+center.y()},
                         {   center.x(), -r+center.y()},
                         { h+center.x(), -r+center.y()},
                         { c+center.x(), -c+center.y()},
                         { r+center.x(), -h+center.y()}};


    moveTo(points[0]);
    if (dir == kCW_Direction)
    {
        for (int i = 15; i - 1 > 0; i -= 2)
        {
            quadTo(points[i].x(), points[i].y(), points[i - 1].x(), points[i - 1].y());
        }
        quadTo(points[1].x(), points[1].y(), points[0].x(), points[0].y());
    }
    else
    {
        for (int i = 1; i + 1 < 16; i += 2)
        {
            quadTo(points[i].x(), points[i].y(), points[i + 1].x(), points[i + 1].y());
        }
        quadTo(points[15].x(), points[15].y(), points[0].x(), points[0].y());
    }
    return *this;
}

void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t)
{
    dst[0] = src[0];
    dst[1] = (1 - t) * src[0] + t * src[1];
    dst[3] = (1 - t) * src[1] + t * src[2];
    dst[2] = (1 - t) * dst[1] + t * dst[3];
    dst[4] = src[2];
}

void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t)
{
    dst[0] = src[0];
    dst[1] = (1 - t) * src[0] + t * src[1];
    dst[5] = (1 - t) * src[2] + t * src[3];
    dst[6] = src[3];

    GPoint bc = (1 - t) * src[1] + t * src[2];
    dst[2] = (1 - t) * dst[1] + t * bc;
    dst[4] = (1 - t) * bc + t * dst[5];
    dst[3] = (1 - t) * dst[2] + t * dst[4];
}