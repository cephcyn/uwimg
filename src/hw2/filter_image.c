#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // 2.1 TODONE
    // calculate image sum
    float sum = 0;
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            for (int c = 0; c < im.c; c++) {
                sum += get_pixel(im, x, y, c);
            }
        }
    }
    // divide all pixels by sum
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            for (int c = 0; c < im.c; c++) {
                set_pixel(im, x, y, c, get_pixel(im, x, y, c) / sum);
            }
        }
    }
}

image make_box_filter(int w)
{
    // 2.1 TODONE
    image f = make_image(w,w,1);
    for (int x = 0; x < f.w; x++) {
        for (int y = 0; y < f.h; y++) {
            set_pixel(f, x, y, 0, 1);
        }
    }
    l1_normalize(f);
    return f;
}

static float get_filtered_value(image im, image f, int x, int y, int c, int cf) {
    float sum = 0;
    int fxw = (f.w-1)/2; 
    int fyw = (f.h-1)/2;
    for (int xf = 0; xf < f.w; xf++) {
        for (int yf = 0; yf < f.h; yf++) {
            sum += (get_pixel(im, x - fxw + xf, y - fyw + yf, c) * get_pixel(f, xf, yf, cf));
        }
    }
    return sum;
    
}

image convolve_image(image im, image filter, int preserve)
{
    // 2.2 TODONE
    image copy = copy_image(im);
    // colvolve each channel
    for (int c = 0; c < im.c; c++) {
        int filter_c = c;
        // if filter has only one channel, resuse it
        if(filter.c == 1) {
            filter_c = 0;
        }
        for (int x = 0; x < im.w; x++) {
            for (int y = 0; y < im.h; y++) {
                set_pixel(copy, x, y, c, get_filtered_value(im, filter, x, y, c, filter_c));
            }
        }
    }
    
    // sum each channel together to get a 1 channel image
    if (!preserve || im.c == 1) {
        image sum = make_image(copy.w, copy.h, 1); 
        for (int x = 0; x < im.w; x++) {
            for (int y = 0; y < im.h; y++) {
                float s = 0;
                for(int c = 0; c < im.c; c++) {
                    s += get_pixel(copy, x, y ,c);
                }
                set_pixel(sum, x, y, 0, s);
            }
        }
        return sum;
    }
    return copy;
}


image make_highpass_filter()
{
    // 2.2? TODONE
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 4);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);
    return filter;
}

image make_sharpen_filter()
{
    // 2.2? TODONE
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, 0);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 5);
    set_pixel(filter, 2, 1, 0, -1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, -1);
    set_pixel(filter, 2, 2, 0, 0);
    return filter;
}

image make_emboss_filter()
{
    // 2.2? TODONE
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -2);
    set_pixel(filter, 1, 0, 0, -1);
    set_pixel(filter, 2, 0, 0, 0);
    set_pixel(filter, 0, 1, 0, -1);
    set_pixel(filter, 1, 1, 0, 1);
    set_pixel(filter, 2, 1, 0, 1);
    set_pixel(filter, 0, 2, 0, 0);
    set_pixel(filter, 1, 2, 0, 1);
    set_pixel(filter, 2, 2, 0, 2);
    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: We want to use preserve for "Box", "Sharpen", and "Emboss" filters because those filters still produce RGB image output. We don't want to use preserve for the "Highpass" filter because since it's intended for edge detection, we would prefer grayscale output.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: We need to normalize for "Sharpen", "Emboss", and "Highpass" filters since they're not normalized, so using them raw could end up giving us pixel values in different channels greater than 1 (e.g. if the pixel is white, it ends up extra-white!). With the same reasoning, we don't need to post-process "Box" filter since it's already normalized. For all of them, the preserve setting takes care of channel logic so we don't need to do any post-processing related to channels. 

image make_gaussian_filter(float sigma)
{
    // 2.3 TODONE
    // Calculate the filter size we need... smallest odd int > 6*sigma
    int size = (int) (2*ceilf((sigma*6 - 1)/2))+1;
    image filter = make_image(size,size,1);
    // set the raw weights
    int center = (size-1)/2;
    float factor = 1.0 / (2.0*M_PI*sigma*sigma);
    for (int x = 0; x <= (size-1)/2; x++) {
        for (int y = 0; y <= (size-1)/2; y++) {
            float exp = expf(-1.0*(x*x + y*y)/(2.0*sigma*sigma));
            float value = factor*exp;
            set_pixel(filter, center+x, center+y, 0, value);
            set_pixel(filter, center+x, center-y, 0, value);
            set_pixel(filter, center-x, center+y, 0, value);
            set_pixel(filter, center-x, center-y, 0, value);
        }
    }
    // normalize the weights
    l1_normalize(filter);
    return filter;
}

image add_image(image a, image b)
{
    // 2.4 TODONE
    // check size; from Redmon's suggestion
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image result = make_image(a.w, a.h, a.c);
    for (int x = 0; x < a.w; x++) {
        for (int y = 0; y < a.h; y++) {
            for (int c = 0; c < a.c; c++) {
                float value_a = get_pixel(a, x, y, c);
                float value_b = get_pixel(b, x, y, c);
                set_pixel(result, x, y, c, value_a+value_b);
            }
        }
    }
    return result;
}

image sub_image(image a, image b)
{
    // 2.4 TODONE
    // check size; from Redmon's suggestion
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image result = make_image(a.w, a.h, a.c);
    for (int x = 0; x < a.w; x++) {
        for (int y = 0; y < a.h; y++) {
            for (int c = 0; c < a.c; c++) {
                float value_a = get_pixel(a, x, y, c);
                float value_b = get_pixel(b, x, y, c);
                set_pixel(result, x, y, c, value_a-value_b);
            }
        }
    }
    return result;
}

image make_gx_filter()
{
    // 2.5.1 TODO
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, 0);
    set_pixel(filter, 2, 0, 0, 1);
    set_pixel(filter, 0, 1, 0, -2);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 2);
    set_pixel(filter, 0, 2, 0, -1);
    set_pixel(filter, 1, 2, 0, 0);
    set_pixel(filter, 2, 2, 0, 1);
    return filter;
}

image make_gy_filter()
{
    // 2.5.1 TODO
    image filter = make_image(3,3,1);
    set_pixel(filter, 0, 0, 0, -1);
    set_pixel(filter, 1, 0, 0, -2);
    set_pixel(filter, 2, 0, 0, -1);
    set_pixel(filter, 0, 1, 0, 0);
    set_pixel(filter, 1, 1, 0, 0);
    set_pixel(filter, 2, 1, 0, 0);
    set_pixel(filter, 0, 2, 0, 1);
    set_pixel(filter, 1, 2, 0, 2);
    set_pixel(filter, 2, 2, 0, 1);
    return filter;
}

void feature_normalize(image im)
{
    // 2.5.2 TODO
    // get the minimum and maximum values for the entire image
    float min = get_pixel(im, 0, 0, 0);
    float max = get_pixel(im, 0, 0, 0);
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            for (int c = 0; c < im.c; c++) {
                float value = get_pixel(im, x, y, c);
                if (value < min) {
                    min = value;
                }
                if (value > max) {
                    max = value;
                }
            }
        }
    }
    // calculate the range
    float range = max - min;
    // set all the new values
    for (int x = 0; x < im.w; x++) {
        for (int y = 0; y < im.h; y++) {
            for (int c = 0; c < im.c; c++) {
                if (range != 0) {
                    // scale the value
                    float value = get_pixel(im, x, y, c);
                    set_pixel(im, x, y, c, (value - min) / range);
                } else {
                    // if the range is 0, just set everything to 0
                    set_pixel(im, x, y, c, 0);
                }
            }
        }
    }
}

image *sobel_image(image im)
{
    // 2.5.3 TODONE
    // based on Redmon's suggestion
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image gx = convolve_image(im, gx_filter, 0);
    image gy = convolve_image(im, gy_filter, 0);
    image *ret = calloc(2, sizeof(image));
    image mag = make_image(im.w, im.h, 1);
    image dir = make_image(im.w, im.h, 1);
    ret[0] = mag;
    ret[1] = dir;

    // loop through gx and gy to populate mag and dir
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            float gx_pix = get_pixel(gx, x, y, 0);
            float gy_pix = get_pixel(gy, x, y, 0);
            float mag_pix = sqrtf((gx_pix * gx_pix) + (gy_pix * gy_pix));
            float dir_pix = atan2(gy_pix, gx_pix);
            set_pixel(mag, x, y, 0, mag_pix);
            set_pixel(dir, x, y, 0, dir_pix);
        }
    }

    return ret;
}

image colorize_sobel(image im)
{
    // 2.5.4 TODONE
    // make it so that as you go:
    // l->r red increases
    // top->bottom increases green
    // blue incrases as average of red & green increases;
    // these all are also porportional to the normalized
    // direction from applying sobel to the provided image
    image creative = make_image(im.w, im.h, 3);
    image* sobel = sobel_image(im);
    image dir = sobel[1];
    feature_normalize(dir);
    for(int x = 0; x < im.w; x++) {
        for(int y = 0; y < im.h; y++) {
            float sobel_pix = get_pixel(dir, x, y, 0);
            float pix_r = sobel_pix * (1.0f + x) / im.w;
            float pix_g = sobel_pix * (1.0f + y) / im.h;
            float pix_b = (pix_g + pix_r / 2.0f);
            set_pixel(creative, x, y, 0, pix_r);
            set_pixel(creative, x, y, 1, pix_g);
            set_pixel(creative, x, y, 2, pix_b);
        }
    }
    return creative;
}
