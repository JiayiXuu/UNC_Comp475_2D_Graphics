/**
 *  Copyright 2015 Mike Reed
 */

#include "image.h"

#include "image_pa1.cpp"
#include "image_pa2.cpp"
#include "image_pa3.cpp"
#include "image_pa4.cpp"
#include "image_pa5.cpp"
#include "image_pa6.cpp"

const GDrawRec gDrawRecs[] = {
    { draw_solid_ramp,  256, 7*28,  "solid_ramp",   1   },
    { draw_graphs,      256, 256,   "rect_graphs",  1   },
    { draw_blend_black, 200, 200,   "blend_black",  1   },

    { draw_poly,        512, 512,   "poly",         2 },
    { draw_poly_center, 256, 256,   "poly_center",  2 },
    { draw_poly_vstrip, 128, 512,   "poly_vstrip",  2 },
    { draw_blendmodes,  450, 340,   "blendmodes",   2 },

    { draw_checker,     300, 300,   "checkers",     3 },
    { draw_poly_rotate, 300, 300,   "color_clock",  3 },
    { draw_bitmaps_hole,300, 300,   "bitmap_hole",  3 },
    { draw_clock_bm,    480, 480,   "spock_clock",  3 },
    { draw_bm_blendmodes,  450, 340,   "blendmodes2",   3 },

    { stars,            512, 512,   "stars",        4 },
    { draw_lion,        512, 512,   "lion",         4 },
    { draw_lion_head,   512, 512,   "lion_head",    4 },
    { draw_grad,        250, 200,   "grad",         4 },
    { draw_gradient_blendmodes, 450, 340, "gradient_blendmodes", 4 },
    { draw_graphs2,     256, 256,   "path_graphs",  4  },

    { draw_quad_rotate, 300, 300,   "quad_clock",  5  },
    { draw_cubics,      512, 512,    "cubic_fan",  5  },
    { draw_rings,       512, 512,   "rings", 5 },
    { draw_bm_tiling,   512, 512,   "bitmap_tiling", 5 },
    { draw_cartman,     512, 512,   "cartman", 5 },
    { draw_divided,     512, 512,   "divided", 5 },

    { draw_tri,         512, 512,   "tri_color",   6 },
    { draw_tri2,        512, 512,   "tri_texture", 6 },
    { mesh_1,           512, 512,   "sweep_mesh",  6 },
    { mesh_3,           512, 512,   "both_mesh",   6 },
    { spock_quad,       512, 512,   "spock_quad",  6 },
    { color_quad,       512, 512,   "color_quad",  6 },

    { nullptr, 0, 0, nullptr },
};
