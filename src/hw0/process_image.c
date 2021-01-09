// Collaborators: Joyce Zhou, Travis McGaha

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // 0.0 TODONE Fill this in
    // x=column(width), y=row(height), c=channel
    // Use clamp padding: move target to closest coord within image bounds
    if (x < 0) {
        x = 0;
    } else if (x >= im.w) {
        x = im.w - 1;
    }
    if (y < 0) {
        y = 0;
    } else if (y >= im.h) {
        y = im.h - 1;
    }
    // channel clamping really shouldn't ever happen
    if (c < 0) {
        printf("get_pixel clamping from negative on c=%d", c);
        c = 0;
    } else if (c >= im.c) {
        printf("get_pixel clamping from positive on c=%d", c);
        c = im.c - 1;
    }
    return im.data[(im.w*im.h*c)+(im.w*y)+(x)];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // 0.0 TODONE Fill this in
    // x=column(width), y=row(height), c=channel
    // If outside bounds, do nothing
    if ((x < 0) || (x >= im.w)) {
        return;
    }
    if ((y < 0) || (y >= im.h)) {
        return;
    }
    if ((c < 0) || (c >= im.c)) {
        return;
    }
    im.data[(im.w*im.h*c)+(im.w*y)+(x)] = v;
    return;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // 0.1 TODO Fill this in
    memcpy(copy.data, im.data, im.w*im.h*im.c);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
}

void clamp_image(image im)
{
    // TODO Fill this in
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
}
