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
    // 1.2 TODO Fill in (also fix that first line)
    return make_image(1,1,1);
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

