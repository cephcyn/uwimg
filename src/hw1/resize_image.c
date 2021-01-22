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
    // 1.3 TODO
    return 0;
}

image bilinear_resize(image im, int w, int h)
{
    // 1.4 TODO
    return make_image(1,1,1);
}

