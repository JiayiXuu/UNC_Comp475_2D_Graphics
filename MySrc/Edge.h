#include "GPoint.h"

class Edge
{
public:
    Edge(GPoint p1, GPoint p2)
    {
        if (p1.fY > p2.fY)
        {
            std::swap(p1, p2);
        }
        ymax = GRoundToInt(p2.fY);
        ymin = GRoundToInt(p1.fY);
        m = (p1.fX - p2.fX) / (p1.fY - p2.fY);
        b = p1.fX - m * p1.fY;
        start_x = p1.fX + m * (ymin - p1.fY + 0.5f);
    }
    Edge(GPoint p1, GPoint p2, int wind)
    {
        if (p1.fY > p2.fY)
        {
            std::swap(p1, p2);
            wind = -wind;
        }
        ymax = GRoundToInt(p2.fY);
        ymin = GRoundToInt(p1.fY);
        m = (p1.fX - p2.fX) / (p1.fY - p2.fY);
        b = p1.fX - m * p1.fY;
        start_x = p1.fX + m * (ymin - p1.fY + 0.5f);
        winding = wind;
        removed = false;
    }

    void setWinding(int new_w)
    {
        this->winding = new_w;
        return;
    }
    int getWinding()
    {
        return this->winding;
    }

    int getYMax()
    {
        return ymax;
    }

    int getYMin()
    {
        return ymin;
    }

    float m, b, start_x;
    int ymax, ymin;
    int winding = 1;
    bool removed;

    bool operator<(Edge edge2) const
    {
        if (ymin != edge2.getYMin())
            return ymin < edge2.getYMin();
        if (this->start_x != edge2.start_x)
            return this->start_x < edge2.start_x;
        return this->m < edge2.m;
    }
};

bool sort_x(Edge a, Edge b)
{
    return a.start_x < b.start_x;
};

void clipEdge(GPoint p1, GPoint p2, int height, int width, std::vector<Edge> &edges)
{
    int wind = 1;
    if (p1.fY > p2.fY) //p1 has smaller y
    {
        std::swap(p1, p2);
        wind = -wind;
    }
    if (GRoundToInt(p1.fY) == GRoundToInt(p2.fY))
    {
        return;
    }
    if (p2.fY > 0 && p1.fY < height)
    {

        if (p1.fY < 0)
        {
            float x1 = p1.x() + (p2.x() - p1.x()) * (-p1.y()) / (p2.y() - p1.y());
            p1.set(x1, 0.f);
        }

        if (p2.fY > height)
        {
            float x2 = p2.x() - (p2.x() - p1.x()) * (p2.y() - height) / (p2.y() - p1.y());
            p2.set(x2, (float)height);
        }

        if (p1.fX > p2.fX)
        {
            std::swap(p1, p2);
            wind = -wind;
        }

        if (p2.fX <= 0)
        {
            p1.set(0, p1.fY);
            p2.set(0, p2.fY);
        }
        else if (p1.fX >= width)
        {
            p1.set((float)width, p1.fY);
            p2.set((float)width, p2.fY);
        }
        else
        {
            GPoint proj;
            if (p1.fX < 0)
            {
                float x1 = p1.y() + (-p1.x()) * (p2.y() - p1.y()) / (p2.x() - p1.x());
                proj.set(0.f, p1.fY);
                p1.set(0.f, x1);
                if (GRoundToInt(proj.fY) != GRoundToInt(p1.fY))
                {
                    edges.push_back(Edge(proj, p1, wind));
                }
            }
            if (p2.fX > width)
            {
                float x2 = p2.y() - (p2.x() - (float)width) * (p2.y() - p1.y()) / (p2.x() - p1.x());
                proj.set((float)width, p2.fY);
                p2.set((float)(width), x2);
                if (GRoundToInt(proj.fY) != GRoundToInt(p2.fY))
                {
                    edges.push_back(Edge(p2, proj, wind));
                }
            }
        }
        if (GRoundToInt(p1.fY) != GRoundToInt(p2.fY))
        {
            edges.push_back(Edge(p1, p2,wind));
        }
    }
    return;
}

void clipHV(const GPoint my_point[], std::vector<Edge> &edges, int height, int width, int count)
{

    int j;
    GPoint p1;
    GPoint p2;

    for (int i = 0; i < count; ++i)
    {

        if (i == count - 1)
        {
            j = 0;
        }
        else
        {
            j = i + 1;
        }

        if (my_point[i].fY > my_point[j].fY)
        {
            p1 = my_point[j];
            p2 = my_point[i];
        }
        else
        {
            p1 = my_point[i];
            p2 = my_point[j];
        }

        if (GRoundToInt(p1.fY) != GRoundToInt(p2.fY) && p2.fY > 0.f && p1.fY < height)
        {

            if (p1.fY <= 0)
            {
                float x1 = p1.x() + (p2.x() - p1.x()) * (-p1.y()) / (p2.y() - p1.y());
                p1.set(x1, 0.f);
            }

            if (p2.fY > height)
            {
                float x2 = p2.x() - (p2.x() - p1.x()) * (p2.y() - height) / (p2.y() - p1.y());
                p2.set(x2, (float)height);
            }

            if (p1.fX > p2.fX)
            {
                std::swap(p1, p2);
            }

            if (p2.fX < 0)
            {
                p1.set(0, p1.fY);
                p2.set(0, p2.fY);
            }
            else if (p1.fX > width)
            {
                p1.set((float)width, p1.fY);
                p2.set((float)width, p2.fY);
            }
            else
            {
                GPoint proj;
                if (p1.fX < 0)
                {
                    float x1 = p1.y() + (-p1.x()) * (p2.y() - p1.y()) / (p2.x() - p1.x());
                    proj.set(0.f, p1.fY);
                    p1.set(0.f, x1);
                    edges.push_back(Edge(p1, proj));
                }
                if (p2.fX > width)
                {
                    float x2 = p2.y() - (p2.x() - (float)width) * (p2.y() - p1.y()) / (p2.x() - p1.x());
                    proj.set((float)width, p2.fY);
                    p2.set((float)(width), x2);
                    edges.push_back(Edge(p2, proj));
                }
            }
            if (GRoundToInt(p1.fY) != GRoundToInt(p2.fY))
            {
                edges.push_back(Edge(p1, p2));
            }
        }
    }
}