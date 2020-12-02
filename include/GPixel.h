/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GPixel_DEFINED
#define GPixel_DEFINED

#include "GTypes.h"

#define GPIXEL_SHIFT_A  24
#define GPIXEL_SHIFT_R  16
#define GPIXEL_SHIFT_G   8
#define GPIXEL_SHIFT_B   0

/**
 *  Defines our 32bit pixel to be just an int. It stores its components based
 *  on the defines GPIXEL_SHIFT_... for each component.
 *
 *  GPixels are in premultiplied form: the red, green, blue compoents have already been
 *  multiplied by their alpha value.
 */
typedef uint32_t GPixel;

///////////////////////////////////////////////////////////////////////////////

static inline int GPixel_GetA(GPixel p) { return (p >> GPIXEL_SHIFT_A) & 0xFF; }
static inline int GPixel_GetR(GPixel p) { return (p >> GPIXEL_SHIFT_R) & 0xFF; }
static inline int GPixel_GetG(GPixel p) { return (p >> GPIXEL_SHIFT_G) & 0xFF; }
static inline int GPixel_GetB(GPixel p) { return (p >> GPIXEL_SHIFT_B) & 0xFF; }

/*
 *  Asserts that a, r, g, b are already in premultiply form, and simiply
 *  packs them into a GPixel.
 */
static inline GPixel GPixel_PackARGB(unsigned a, unsigned r, unsigned g, unsigned b) {
    // These asserts check that the components are "legal" for premultiplied colors
    assert(a <= 255);
    assert(r <= a);
    assert(g <= a);
    assert(b <= a);
    
    return  (a << GPIXEL_SHIFT_A) |
            (r << GPIXEL_SHIFT_R) |
            (g << GPIXEL_SHIFT_G) |
            (b << GPIXEL_SHIFT_B);
}

#endif
