#include <stack>

#include "GCanvas.h"
#include "GPaint.h"
#include "GBitmap.h"
#include "Myutils.h"
#include "GRect.h"
#include "Edge.h"
#include "GMatrix.h"
#include "GShader.h"
#include "GColor.h"
#include "GPath.h"
#include "GBlitter.h"

GShader *GCreatProxyShader(GShader *Shader, GPoint p0, GPoint p1, GPoint p2, GPoint s0, GPoint s1, GPoint s2);
GShader *GCreateComposeShader(GShader *Shader0, GShader *Shader1);
GShader *GCreatTriColorShader(GPoint p0, GPoint p1, GPoint p2, GColor c0, GColor c1, GColor c2);

class MyCanvas : public GCanvas
{
public:
    MyCanvas(const GBitmap &device) : fDevice(device)
    {
        m_stack.push(CTM);
    }

    void save()
    {
        GMatrix current = CTM;
        m_stack.push(current);
    }

    void restore()
    {
        CTM = m_stack.top();
        m_stack.pop();
    }

    void concat(const GMatrix &matrix)
    {
        CTM.preConcat(matrix);
    }

    void drawPaint(const GPaint &paint) override
    {
        drawRect(GRect::MakeWH(fDevice.width(), fDevice.height()), paint);
    }
    void drawRect(const GRect &rect, const GPaint &paint) override
    {
        GPoint my_points[4] = {
            GPoint::Make(rect.left(), rect.top()),
            GPoint::Make(rect.right(), rect.top()),
            GPoint::Make(rect.right(), rect.bottom()),
            GPoint::Make(rect.left(), rect.bottom())};

        drawConvexPolygon(my_points, 4, paint);
    }

    /**
     *  Fill the convex polygon with the color, following the same "containment" rule as
     *  rectangles.
     */

    void drawConvexPolygon(const GPoint my_point[], int count, const GPaint &paint)
    {
        //check count
        if (count <= 2)
        {
            return;
        }
        //process shader, transform points
        GShader *shader = paint.getShader();
        if (shader != nullptr && !(shader->setContext(CTM)))
        {
            return;
        }
        GPoint points[count];
        CTM.mapPoints(points, my_point, count);

        //process blendmode
        BlendFunc b_func;
        ScanFunction s_func;
        if (shader != nullptr)
        {
            b_func = switch_mode(paint, shader, shader->isOpaque());
            s_func = ShadeSpan;
        }
        else
        {
            b_func = switch_mode(paint, shader, false);
            s_func = Span;
        }
        if (b_func == Dst_mode)
        {
            return;
        }

        GPixel src = MakeGPixel(paint.getColor());

        // clip and sort
        std::vector<Edge> edges;
        int height = fDevice.height();
        int width = fDevice.width();
        clipHV(points, edges, height, width, count);

        if (edges.size() < 2)
        {
            return;
        }
        std::sort(edges.begin(), edges.end());

        // blit and scan
        int index = 2;
        Edge edge1 = edges.at(0);
        Edge edge2 = edges.at(1);

        int y = edge1.getYMin();
        int y_bottom = edges.at(edges.size() - 1).getYMax();

        while (y < y_bottom)
        {

            if (y >= edge1.getYMax())
            {
                edge1 = edges.at(index);
                index++;
            }
            if (y >= edge2.getYMax())
            {
                edge2 = edges.at(index);
                index++;
            }
            s_func(GRoundToInt(edge1.start_x), GRoundToInt(edge2.start_x), y, shader, fDevice, b_func, src);

            edge1.start_x += edge1.m;
            edge2.start_x += edge2.m;
            y++;
        }
    }

    void drawPath(const GPath &path, const GPaint &paint)
    {
        //process shader, transform points
        GShader *shader = paint.getShader();
        if (shader != nullptr && !(shader->setContext(CTM)))
        {
            return;
        }
        BlendFunc b_func;
        ScanFunction s_func;
        if (shader != nullptr)
        {
            b_func = switch_mode(paint, shader, shader->isOpaque());
            s_func = ShadeSpan;
        }
        else
        {
            b_func = switch_mode(paint, shader, false);
            s_func = Span;
        }
        if (b_func == Dst_mode)
        {
            return;
        }
        GPixel src = MakeGPixel(paint.getColor());

        // transform point
        GPath mypath = path;
        mypath.transform(CTM);
        //get edges
        std::vector<Edge> edges;

        GPoint pts[GPath::kMaxNextPoints]; // enough storage for each call to next()
        GPath::Edger iter(mypath);

        int height = fDevice.height();
        int width = fDevice.width();
        GPath::Verb v = iter.next(pts);
        while (v != GPath::kDone)
        {
            switch (v)
            {
            case GPath::kLine:
            {
                clipEdge(pts[0], pts[1], height, width, edges);
                break;
            }
            case GPath::kQuad:
            {
                //calculate number of edges
                GVector error = 0.25 * ((-1) * pts[0] + 2 * pts[1] - pts[2]);
                int count = GCeilToInt(sqrt(error.length() * 4.f));
                float t_1 = 1.f / count;
                //coefficents
                GPoint co_a = pts[0] + (-2) * pts[1] + pts[2];
                GPoint co_b = 2 * ((-1) * pts[0] + pts[1]);
                GPoint co_c = pts[0];
                //add edges
                GPoint p0 = pts[0];
                GPoint p1;
                for (float t = t_1; t < 1.f; t += t_1)
                {
                    p1 = (co_a * t + co_b) * t + co_c;
                    clipEdge(p0, p1, height, width, edges);
                    p0 = p1;
                }
                clipEdge(p0, (co_a + co_b) + co_c, height, width, edges);
                break;
            }
            case GPath::kCubic:
            {
                GPoint p = pts[0] + (-2) * pts[1] + pts[2];
                GPoint q = pts[1] + (-2) * pts[2] + pts[3];
                GVector error;
                error.set(std::max(abs(p.fX), abs(q.fX)), std::max(abs(p.fY), abs(q.fY)));
                int count = GCeilToInt(sqrt(3.f * error.length()));
                //add edges
                float t_1 = 1.f / count;
                GPoint co_a = (-1) * pts[0] + 3 * pts[1] + (-3) * pts[2] + pts[3];
                GPoint co_b = 3 * pts[0] + (-6) * pts[1] + 3 * pts[2];
                GPoint co_c = 3 * (pts[1] + (-1) * pts[0]);
                GPoint co_d = pts[0];
                GPoint p0 = pts[0];
                GPoint p1;
                for (float t = t_1; t < 1.f; t += t_1)
                {
                    p1 = ((co_a * t + co_b) * t + co_c) * t + co_d;
                    clipEdge(p0, p1, height, width, edges);
                    p0 = p1;
                }
                clipEdge(p0, ((co_a + co_b) + co_c) + co_d, height, width, edges);
                break;
            }
            default:
            {
                break;
            }
            }
            v = iter.next(pts);
        }

        if (edges.size() < 2)
        {
            return;
        }

        std::sort(edges.begin(), edges.end());

        GRect bound = mypath.bounds();
        int y_top = GRoundToInt(bound.fTop);
        int y_bottom = GRoundToInt(bound.fBottom);

        std::vector<Edge> active_Edges;
        int index = 0;
        for (int y = y_top; y < y_bottom; y++)
        {
            for (int i = index; i < edges.size(); i++)
            {
                if (edges.at(i).getYMin() == y)
                {
                    active_Edges.push_back(edges.at(i));
                    index = i + 1;
                }
            }
            std::sort(active_Edges.begin(), active_Edges.end(), sort_x);
            int winding = 0;
            int x0, x1;
            for (int i = 0; i < active_Edges.size(); i++)
            {
                if (winding == 0)
                {
                    x0 = GRoundToInt(active_Edges.at(i).start_x);
                }
                winding += active_Edges.at(i).winding;
                if (winding == 0)
                {
                    x1 = GRoundToInt(active_Edges.at(i).start_x);
                    s_func(x0, x1, y, shader, fDevice, b_func, src);
                }
                if (active_Edges[i].getYMax() == y + 1)
                {
                    // active_Edges.erase(active_Edges.begin() + i);
                    active_Edges[i].removed = true;
                }
                else
                {
                    active_Edges[i].start_x += active_Edges[i].m;
                }
            }
            std::vector<Edge> cleaned;
            for (int i = 0; i < active_Edges.size(); i++)
            {
                if (!active_Edges[i].removed)
                    cleaned.push_back(active_Edges[i]);
            }
            active_Edges = cleaned;
        }
    }
    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint &paint)
    {
        GPoint triangle[3];
        GPoint tex[3];
        GColor color[3];
        GPaint my_paint = paint;

        GShader *shader;
        GShader *shader_c;
        GShader *shader_b;
        GShader *shader_compose;
        int n = 0;

        if (colors != nullptr && texs == nullptr)
        {
            for (int i = 0; i < count; ++i)
            {
                triangle[0] = verts[indices[n + 0]];
                triangle[1] = verts[indices[n + 1]];
                triangle[2] = verts[indices[n + 2]];
                color[0] = colors[indices[n + 0]];
                color[1] = colors[indices[n + 1]];
                color[2] = colors[indices[n + 2]];
                shader = GCreatTriColorShader(triangle[0], triangle[1], triangle[2], color[0], color[1], color[2]);
                shader->setContext(CTM);
                my_paint.setShader(shader);

                Tri_Scanner(3, triangle, my_paint, shader);
                n += 3;
            }
        }
        else if (colors == nullptr && texs != nullptr)
        {
            for (int i = 0; i < count; ++i)
            {
                triangle[0] = verts[indices[n + 0]];
                triangle[1] = verts[indices[n + 1]];
                triangle[2] = verts[indices[n + 2]];
                tex[0] = texs[indices[n + 0]];
                tex[1] = texs[indices[n + 1]];
                tex[2] = texs[indices[n + 2]];
                shader = GCreatProxyShader(paint.getShader(), triangle[0], triangle[1], triangle[2], tex[0], tex[1], tex[2]);
                shader->setContext(CTM);
                Tri_Scanner(3, triangle, my_paint, shader);
                n += 3;
            }
        }
        else if (colors != nullptr && texs != nullptr)
        {
            for (int i = 0; i < count; ++i)
            {
                triangle[0] = verts[indices[n + 0]];
                triangle[1] = verts[indices[n + 1]];
                triangle[2] = verts[indices[n + 2]];

                color[0] = colors[indices[n + 0]];
                color[1] = colors[indices[n + 1]];
                color[2] = colors[indices[n + 2]];
                shader_c = GCreatTriColorShader(triangle[0], triangle[1], triangle[2], color[0], color[1], color[2]);

                tex[0] = texs[indices[n + 0]];
                tex[1] = texs[indices[n + 1]];
                tex[2] = texs[indices[n + 2]];
                shader_b = GCreatProxyShader(paint.getShader(), triangle[0], triangle[1], triangle[2], tex[0], tex[1], tex[2]);

                shader_compose = GCreateComposeShader(shader_c, shader_b);
                shader = shader_compose;
                shader->setContext(CTM);
                Tri_Scanner(3, triangle, my_paint, shader);
                n += 3;
            }
        }

        return;
    }

    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4], int level, const GPaint &paint)
    {
        if (colors == nullptr && texs == nullptr)
        {
            return;
        }
        GColor ca, cb, cc, cd;
        if (colors != nullptr)
        {
            ca = colors[0].pinToUnit();
            cb = colors[1].pinToUnit();
            cc = colors[2].pinToUnit();
            cd = colors[3].pinToUnit();
        }

        GPoint tA, tB, tC, tD;
        if (texs != nullptr)
        {
            tA = texs[0];
            tB = texs[1];
            tC = texs[2];
            tD = texs[3];
        }

        GPoint A = verts[0];
        GPoint B = verts[1];
        GPoint C = verts[2];
        GPoint D = verts[3];

        int num_of_coor = level + 2;
        int vert_coor = num_of_coor * num_of_coor;
        int num_of_tri_block = 2 * (level + 1);
        int num_of_tri = num_of_tri_block * (level + 1);
        int num_of_vertex_block = num_of_tri_block * 3;
        int num_of_vertex = num_of_tri * 3;

        int *new_indices = (int *)malloc(num_of_vertex * sizeof(int));

        new_indices[0] = 0;
        new_indices[1] = 1;
        new_indices[2] = num_of_coor;
        new_indices[3] = 1;
        new_indices[4] = num_of_coor;
        new_indices[5] = num_of_coor + 1;

        for (int i = 6; i < num_of_vertex_block; i++)
        {
            new_indices[i] = new_indices[i % 6] + i / 6;
        }
        for (int i = num_of_vertex_block; i < num_of_vertex; i++)
        {
            new_indices[i] = new_indices[i % num_of_vertex_block] + (i / num_of_vertex_block) * num_of_coor;
        }

        float du = 1.0f / (float)(level + 1);
        float dv = 1.0f / (float)(level + 1);
        float u = 0;
        float v = 0;
        GPoint *new_verts = (GPoint *)malloc(vert_coor * sizeof(GPoint));
        GPoint *new_tex = (GPoint *)malloc(vert_coor * sizeof(GPoint));
        GColor *new_colors = (GColor *)malloc(vert_coor * sizeof(GColor));

        if (texs != nullptr && colors != nullptr)
        {
            float a_co, b_co, c_co, d_co;
            for (int i = 0; i < num_of_coor; i++)
            {
                v = 0;
                for (int j = 0; j < num_of_coor; j++)
                {
                    a_co = (1 - u) * (1 - v);
                    b_co = u * (1 - v);
                    c_co = u * v;
                    d_co = v * (1 - u);
                    new_tex[i * num_of_coor + j] = a_co * tA + b_co * tB + c_co * tC + d_co * tD;
                    new_verts[i * num_of_coor + j] = a_co * A + b_co * B + c_co * C + d_co * D;
                    new_colors[i * num_of_coor + j] = GColor::MakeARGB(
                        a_co * ca.fA + b_co * cb.fA + c_co * cc.fA + d_co * cd.fA,
                        a_co * ca.fR + b_co * cb.fR + c_co * cc.fR + d_co * cd.fR,
                        a_co * ca.fG + b_co * cb.fG + c_co * cc.fG + d_co * cd.fG,
                        a_co * ca.fB + b_co * cb.fB + c_co * cc.fB + d_co * cd.fB);

                    v += dv;
                }
                u += du;
            }

            drawMesh(new_verts, new_colors, new_tex, num_of_tri, new_indices, paint);
            return;
        }
        else if (texs != nullptr)
        {
            for (int i = 0; i < num_of_coor; i++)
            {
                v = 0;
                for (int j = 0; j < num_of_coor; j++)
                {
                    new_tex[i * num_of_coor + j] = (1 - u) * (1 - v) * tA + u * (1 - v) * tB + u * v * tC + v * (1 - u) * tD;
                    new_verts[i * num_of_coor + j] = (1 - u) * (1 - v) * A + u * (1 - v) * B + u * v * C + v * (1 - u) * D;
                    v += dv;
                }
                u += du;
            }
            drawMesh(new_verts, nullptr, new_tex, num_of_tri, new_indices, paint);
            return;
        }
        else if (colors != nullptr)
        {
            float a_co, b_co, c_co, d_co;
            for (int i = 0; i < num_of_coor; i++)
            {
                v = 0;
                for (int j = 0; j < num_of_coor; j++)
                {
                    a_co = (1 - u) * (1 - v);
                    b_co = u * (1 - v);
                    c_co = u * v;
                    d_co = v * (1 - u);
                    new_verts[i * num_of_coor + j] = a_co * A + b_co * B + c_co * C + d_co * D;
                    new_colors[i * num_of_coor + j] = GColor::MakeARGB(
                        a_co * ca.fA + b_co * cb.fA + c_co * cc.fA + d_co * cd.fA,
                        a_co * ca.fR + b_co * cb.fR + c_co * cc.fR + d_co * cd.fR,
                        a_co * ca.fG + b_co * cb.fG + c_co * cc.fG + d_co * cd.fG,
                        a_co * ca.fB + b_co * cb.fB + c_co * cc.fB + d_co * cd.fB);

                    v += dv;
                }
                u += du;
            }
            drawMesh(new_verts, new_colors, nullptr, num_of_tri, new_indices, paint);
            return;
        }
    }

    void Tri_Scanner(int count, GPoint pts[], GPaint &paint, GShader *shader)
    {
        GPoint points[count];
        CTM.mapPoints(points, pts, 3);
        std::vector<Edge> edges;
        int height = fDevice.height();
        int width = fDevice.width();
        clipHV(points, edges, height, width, 3);

        if (edges.size() < 2)
        {
            return;
        }
        std::sort(edges.begin(), edges.end());

        // blit and scan
        int index = 2;
        Edge edge1 = edges.at(0);
        Edge edge2 = edges.at(1);

        int y = edge1.getYMin();
        int y_bottom = edges.at(edges.size() - 1).getYMax();

        while (y < y_bottom)
        {

            if (y >= edge1.getYMax())
            {
                edge1 = edges.at(index);
                index++;
            }
            if (y >= edge2.getYMax())
            {
                edge2 = edges.at(index);
                index++;
            }
            SpanTri(GRoundToInt(edge1.start_x), GRoundToInt(edge2.start_x), y, shader, fDevice);

            edge1.start_x += edge1.m;
            edge2.start_x += edge2.m;
            y++;
        }
    }

private:
    const GBitmap fDevice;
    std::stack<GMatrix> m_stack;
    GMatrix CTM = GMatrix(); //set default identity matrix
};

std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap &device)
{
    if (!device.pixels())
    {
        return nullptr;
    }
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

std::string GDrawSomething(GCanvas *canvas, GISize dim)
{
    int circle = 10;
    float offset = 180.f / (float)circle;
    float radius = 180.f;
    // 	20.8, 76.1, 91
    // 96.1, 98.8, 100
    float r = 0.075f;
    float g = 0.023f;
    float b = 0.001f;
    GColor blue = GColor::MakeARGB(1.0, 0.208, 0.761f, 0.91);
    for (int i = 0; i <= circle; i++)
    {
        GPath path;
        path.addCircle({128, 128}, radius, GPath::kCW_Direction);
        canvas->drawPath(path, GPaint(blue));
        radius -= offset;
        blue = GColor::MakeARGB(1.0f, blue.fR + r, blue.fG + g, blue.fB + b).pinToUnit();
    }

    // GPath path;
    // path.addCircle({50, 50}, 20, GPath::kCW_Direction);
    // canvas->drawPath(path, GPaint(GColor::MakeARGB(1.f, 0.2, 0.3, 0.5)));

    canvas->save();
    canvas->rotate(0.3);
    canvas->scale(0.5, 0.5);
    canvas->translate(193, 47);
    const GPoint vert[12] = {
        {128, 128},
        {138, 5},
        {251, -10},

        {128, 128},
        {251, 138},
        {266, 251},

        {128, 128},
        {118, 251},
        {5, 266},

        {128, 128},
        {5, 118},
        {-10, 5},
    };
    GColor colors[12];
    for (int i = 0; i < 12; i++)
    {
        colors[i] = GColor::MakeARGB(1.f, ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)));
    }
    const int indices[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    GPaint paint = GPaint(GColor::MakeARGB(1, 0, 0, 0));
    canvas->drawMesh(vert, colors, nullptr, 4, indices, paint);

    canvas->restore();
    canvas->save();
    canvas->rotate(0.6);
    canvas->scale(0.3, 0.3);
    canvas->translate(470, -5);
    for (int i = 0; i < 12; i++)
    {
        colors[i] = GColor::MakeARGB(0.7f, ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)));
    }
    canvas->drawMesh(vert, colors, nullptr, 4, indices, paint);


    canvas->translate(94, 6);
    canvas->rotate(0.5);
    canvas->scale(0.7, 0.7);
    for (int i = 0; i < 12; i++)
    {
        colors[i] = GColor::MakeARGB(0.5f, ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)), ((float)rand() / (RAND_MAX)));
    }
    canvas->drawMesh(vert, colors, nullptr, 4, indices, paint);
    return "Windmill";
}
