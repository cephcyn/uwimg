// Collaborators: Joyce Zhou, Travis McGaha

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c) {
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

void set_pixel(image im, int x, int y, int c, float v) {
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
    im.data[(im.w * im.h * c) + (im.w * y) + (x)] = v;
    return;
}

image copy_image(image im) {
    image copy = make_image(im.w, im.h, im.c);
    // 0.1 TODONE Fill this in
    memcpy(copy.data, im.data, im.w * im.h * im.c * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im) {
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

void shift_image(image im, int c, float v) {
    // 0.3 TODONE Fill this in
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float newvalue = get_pixel(im, x, y, c) + v;
            set_pixel(im, x, y, c, newvalue);
        }
    }
}

void clamp_image(image im) {
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
float three_way_max(float a, float b, float c) {
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c) {
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im) {
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
            float hp;
            if (c==0) {
                hp = 0;
            } else if (v==r) {
                hp = (g - b) / c;
            } else if (v==g) {
                hp = (b - r) / c + 2;
            } else { // if v==b
                hp = (r - g) / c + 4;
            }
            float h;
            if (hp<0) {
                h = hp / 6 + 1;
            } else {
                h = hp / 6;
            }
            // Apply the values
            set_pixel(im, x, y, 0, h);
            set_pixel(im, x, y, 1, s);
            set_pixel(im, x, y, 2, v);
        }
    }
}


// Custom function for hsv_to_rgb
// Assumes H in [0, 360) space
// Assumes S in [0, 1) space
// Assumes V in [0, 1) space
float hsv_rgb_transform_func(int n, float h, float s, float v) {
    float k = fmodf(n + (h / 60), 6);
    return v - v * s * fmaxf(0, three_way_min(k, 4 - k, 1));
}

void hsv_to_rgb(image im) {
    // 0.6 TODONE Fill this in
    // Reference for this algorithm: 
    // https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB_alternative
    // channels are (0,1,2)=(H,S,V)
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            // get the hsv values to start with
            float h = get_pixel(im, x, y, 0);
            h = h * 360; // work with a hue in [0, 360) space
            float s = get_pixel(im, x, y, 1);
            float v = get_pixel(im, x, y, 2);
            // calculate the rgb values
            float r = hsv_rgb_transform_func(5, h, s, v);
            float g = hsv_rgb_transform_func(3, h, s, v);
            float b = hsv_rgb_transform_func(1, h, s, v);
            // Apply the values
            set_pixel(im, x, y, 0, r);
            set_pixel(im, x, y, 1, g);
            set_pixel(im, x, y, 2, b);
        }
    }
}

/*
 * 0.7 scale_image
 *
 */
void scale_image(image im, int c, float v) {
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float newvalue = get_pixel(im, x, y, c) * v;
            set_pixel(im, x, y, c, newvalue);
        }
    }
}

/*
 * 0.8 RGB to HCL.
 * This process involves converting to two intermediate colourspaces.
 * gamma decompression
 * RBG -> CIEXYZ
 * CIEXYZ -> CIELUV
 * CIELUV -> HCL
 * reference: https://observablehq.com/@mbostock/luv-and-hcl
 
   https://cscheid.net/2012/02/16/hcl-color-space-blues.html
 */

// https://observablehq.com/@mbostock/lab-and-rgb
// TODO: this may be wrong http://www.brucelindbloom.com/index.html?Eqn_RGB_to_XYZ.html
static float rgb_gamma_transform_func(float val) {
    if (val  <= 0.04045) {
        return val / 12.92f;
    } else {
        return powf(((val + 0.055f) / 1.055f), 2.4f);
    }
}

// http://www.brucelindbloom.com/index.html?Eqn_RGB_to_XYZ.html
static void rgb_gamma_decompress(image im) {
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            // get the rgb values to start with
            float r = get_pixel(im, x, y, 0);
            float g = get_pixel(im, x, y, 1);
            float b = get_pixel(im, x, y, 2);
            // calculate the xyz values
            float r_gamma = rgb_gamma_transform_func(r);
            float g_gamma = rgb_gamma_transform_func(g);
            float b_gamma = rgb_gamma_transform_func(b);
            // Apply the values
            set_pixel(im, x, y, 0, r_gamma);
            set_pixel(im, x, y, 1, g_gamma);
            set_pixel(im, x, y, 2, b_gamma);
        }
    }
}

// Takes in pointeres to the linearized rgb,
// returns as output through the pointers, x, y and z
// https://www.image-engineering.de/library/technotes/958-how-to-convert-between-srgb-and-ciexyz
static void rgb_xyz_transform_func(float* r, float* g, float* b) {
    float sr1 = 0.4124564f * (*r);
    float sr2 = 0.2126729f * (*r);
    float sr3 = 0.0193339f * (*r);
    float sg1 = 0.3575761f * (*g);
    float sg2 = 0.7151522f * (*g);
    float sg3 = 0.1191920f * (*g);
    float sb1 = 0.1804375f * (*b);
    float sb2 = 0.0721750f * (*b);
    float sb3 = 0.9503041f * (*b);
    *r = sr1 + sg1 + sb1;
    *g = sr2 + sg2 + sb2;
    *b = sr3 + sg3 + sb3;
}

// http://www.brucelindbloom.com/index.html?Eqn_RGB_to_XYZ.html
static void rgb_to_xyz(image im) {
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            // get the rgb values to start with
            float r = get_pixel(im, x, y, 0);
            float g = get_pixel(im, x, y, 1);
            float b = get_pixel(im, x, y, 2);
            // calculate the xyz values
            rgb_xyz_transform_func(&r, &g, &b);
            // Apply the values
            set_pixel(im, x, y, 0, r);
            set_pixel(im, x, y, 1, g);
            set_pixel(im, x, y, 2, b);
        }
    }
}

// Takes in pointeres to x, y and z,
// returns as output through the pointers, l, u and v
// http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_Luv.html
static void xyz_luv_transform_func(float* x, float* y, float* z, float ref_white[3]) {
    float y_r = *y / ref_white[0];
    float denom_prime =  (*x + 15 * (*y) + 3 * (*z));
    float u_prime = (4 * (*x)) / denom_prime;
    float v_prime = (9 * (*y)) / denom_prime;
    float denom_prime_r =  (ref_white[0] + 15 * ref_white[1] + 3 * ref_white[2]);
    float u_prime_r = (4 * ref_white[0]) / denom_prime_r;
    float v_prime_r = (9 * ref_white[1]) / denom_prime_r;
    float e = 0.008856;
    float k = 903.3;
    
    if (y_r > e) {
        *x = 116.0f * powf(y_r, (1.0f/3.0f)) - 16.0f;
    } else {
        *x = k * y_r;
    }
    *y = 13 * (*x) * (u_prime - u_prime_r);
    *z = 13 * (*x) * (v_prime - v_prime_r);
}

// http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_Luv.html
static void xyz_to_luv(image im) {
    float ref_white[3] = {255.0f, 255.0f, 255.0f};
    rgb_xyz_transform_func(&(ref_white[0]), &(ref_white[1]), &(ref_white[2]));
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            // get the rgb values to start with
            float X = get_pixel(im, x, y, 0);
            float Y = get_pixel(im, x, y, 1);
            float Z = get_pixel(im, x, y, 2);
            // calculate the xyz values
            xyz_luv_transform_func(&X, &Y, &Z, ref_white);
            // Apply the values
            set_pixel(im, x, y, 0, X);
            set_pixel(im, x, y, 1, Y);
            set_pixel(im, x, y, 2, Z);
        }
    }
}

// "Finally, HCL is then obtained by simply transforming the UV coordinates of Luv to polar coordinates." 
// https://cscheid.net/2012/02/16/hcl-color-space-blues.html
static void luv_to_hcl(image im) {
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            // get the rgb values to start with
            float l = get_pixel(im, x, y, 0);
            float u = get_pixel(im, x, y, 1);
            float v = get_pixel(im, x, y, 2);
            // calculate the h and c
            // h = phase
            // c = length
            float c =  powf((powf(u, 2.0f) + powf(v, 2.0f)), 0.5f);
            // TODO: C atan2 is in radians, idk if we need to convert to degrees
            float h = atan2(v, u);
            if (h < 0) {
              h += M_PI + M_PI;
            }
            // Apply the values
            set_pixel(im, x, y, 0, h);
            set_pixel(im, x, y, 1, c);
            set_pixel(im, x, y, 2, l);
        }
    }
}

void rgb_to_hcl(image im) {
    rgb_gamma_decompress(im);
    rgb_to_xyz(im);
    xyz_to_luv(im);
    luv_to_hcl(im);
}
