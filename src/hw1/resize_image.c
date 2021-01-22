#include <math.h>
#include "image.h"

float nn_interpolate(image im, float x, float y, int c)
{
    // 1.1 TODONE Fill in
    int nx = (int) roundf(x);
    int ny = (int) roundf(y);
    return get_pixel(im, nx, ny, c);
}

image nn_resize(image im, int w, int h)
{
    // 1.2 TODONE Fill in (also fix that first line)
    image nim = make_image(w,h,im.c);
    float rw = ((float) im.w) / w;
    float rh = ((float) im.h) / h;
    for (int y = 0; y < nim.h; y++) {
        for (int x = 0; x < nim.w; x++) {
            for (int c = 0; c < nim.c; c++) {
                float p = nn_interpolate(im, (x+0.5)*rw-0.5, (y+0.5)*rh-0.5, c);
                set_pixel(nim, x, y, c, p);
            }
        }
    }
    return nim;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // 1.3 TODONE
    float v1 = get_pixel(im, (int) floorf(x), (int) floorf(y), c);
    float v2 = get_pixel(im, (int) ceilf(x), (int) floorf(y), c);
    float v3 = get_pixel(im, (int) floorf(x), (int) ceilf(y), c);
    float v4 = get_pixel(im, (int) ceilf(x), (int) ceilf(y), c);
    float d1 = x - floorf(x);
    float d2 = ceilf(x) - x;
    float d3 = y - floorf(y);
    float d4 = ceilf(y) - y;
    float q1 = v1*d2 + v2*d1;
    float q2 = v3*d2 + v4*d1;
    return q1*d4 + q2*d3;
}

image bilinear_resize(image im, int w, int h)
{
    // 1.4 TODONE
    image nim = make_image(w,h,im.c);
    float rw = ((float) im.w) / w;
    float rh = ((float) im.h) / h;
    for (int y = 0; y < nim.h; y++) {
        for (int x = 0; x < nim.w; x++) {
            for (int c = 0; c < nim.c; c++) {
                float p = bilinear_interpolate(im, (x+0.5)*rw-0.5, (y+0.5)*rh-0.5, c);
                set_pixel(nim, x, y, c, p);
            }
        }
    }
    return nim;
}

