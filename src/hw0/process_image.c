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
        fprintf(stderr, "get_pixel clamping from negative on c=%d", c);
        c = 0;
    } else if (c >= im.c) {
        fprintf(stderr, "get_pixel clamping from positive on c=%d", c);
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
        // this is a weird thing to be out of bounds on
        fprintf(stderr, "set_pixel out of bounds on c=%d", c);
        return;
    }
    im.data[(im.w*im.h*c)+(im.w*y)+(x)] = v;
    return;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // 0.1 TODONE Fill this in
    memcpy(copy.data, im.data, im.w*im.h*im.c*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // 0.2 TODONE Fill this in
    // channels are (0,1,2)=(R,G,B)
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float weighted = 0;
            // we want these weights because perception != direct rgb scale
            weighted += 0.299 * get_pixel(im, x, y, 0);
            weighted += 0.587 * get_pixel(im, x, y, 1);
            weighted += 0.114 * get_pixel(im, x, y, 2);
            set_pixel(gray, x, y, 0, weighted);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // 0.3 TODONE Fill this in
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float newvalue = get_pixel(im, x, y, c) + v;
            set_pixel(im, x, y, c, newvalue);
        }
    }
}

void clamp_image(image im)
{
    // 0.4 TODONE Fill this in
    for (int c = 0; c < im.c; c++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float value = get_pixel(im, x, y, c);
                if (value < 0) {
                    value = 0;
                } else if (value > 1) {
                    value = 1;
                }
                set_pixel(im, x, y, c, value);
            }
        }
    }
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
    // 0.5 TODONE Fill this in
    // channels are (0,1,2)=(R,G,B)
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            // get the rgb values to start with
            float r = get_pixel(im, x, y, 0);
            float g = get_pixel(im, x, y, 1);
            float b = get_pixel(im, x, y, 2);
            // calculate Value
            float v = three_way_max(r, g, b);
            // calculate Saturation
            float m = three_way_min(r, g, b);
            float c = v - m;
            float s;
            if (r==0 && g==0 && b==0) {
                s = 0;
            } else {
                s = c / v;
            }
            // calculate Hue
            float h;
            if (c==0) {
                h = 0;
            } else if (v==r) {
                h = (g - b) / c;
            } else if (v==g) {
                h = (b - r) / c + 2;
            } else { // if v==b
                h = (r - g) / c + 4;
            }
            if (h<0) {
                h = h / 6 + 1;
            } else {
                h = h / 6;
            }
            // Apply the values
            set_pixel(im, x, y, 0, h);
            set_pixel(im, x, y, 1, s);
            set_pixel(im, x, y, 2, v);
        }
    }
}

void hsv_to_rgb(image im)
{
    // TODO Fill this in
}
