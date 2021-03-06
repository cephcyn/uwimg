#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#include "matrix.h"
#include <time.h>

// Frees an array of descriptors.
// descriptor *d: the array.
// int n: number of elements in array.
void free_descriptors(descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        free(d[i].data);
    }
    free(d);
}

// Create a feature descriptor for an index in an image.
// image im: source image.
// int i: index in image for the pixel we want to describe.
// returns: descriptor for that index.
descriptor describe_index(image im, int i)
{
    int w = 5;
    descriptor d;
    d.p.x = i%im.w;
    d.p.y = i/im.w;
    d.data = calloc(w*w*im.c, sizeof(float));
    d.n = w*w*im.c;
    int c, dx, dy;
    int count = 0;
    // If you want you can experiment with other descriptors
    // This subtracts the central value from neighbors
    // to compensate some for exposure/lighting changes.
    for(c = 0; c < im.c; ++c){
        float cval = im.data[c*im.w*im.h + i];
        for(dx = -w/2; dx < (w+1)/2; ++dx){
            for(dy = -w/2; dy < (w+1)/2; ++dy){
                float val = get_pixel(im, i%im.w+dx, i/im.w+dy, c);
                d.data[count++] = cval - val;
            }
        }
    }
    return d;
}

// Marks the spot of a point in an image.
// image im: image to mark.
// ponit p: spot to mark in the image.
void mark_spot(image im, point p)
{
    int x = p.x;
    int y = p.y;
    int i;
    for(i = -9; i < 10; ++i){
        set_pixel(im, x+i, y, 0, 1);
        set_pixel(im, x, y+i, 0, 1);
        set_pixel(im, x+i, y, 1, 0);
        set_pixel(im, x, y+i, 1, 0);
        set_pixel(im, x+i, y, 2, 1);
        set_pixel(im, x, y+i, 2, 1);
    }
}

// Marks corners denoted by an array of descriptors.
// image im: image to mark.
// descriptor *d: corners in the image.
// int n: number of descriptors to mark.
void mark_corners(image im, descriptor *d, int n)
{
    int i;
    for(i = 0; i < n; ++i){
        mark_spot(im, d[i].p);
    }
}

// Creates a 1d Gaussian filter.
// float sigma: standard deviation of Gaussian.
// returns: single row image of the filter.
image make_1d_gaussian(float sigma)
{
    // TODONE: optional, make separable 1d Gaussian.
    int size = (int) (2*ceilf((sigma*6 - 1)/2))+1;
    image filter = make_image(size,1,1);
    int center = (size-1)/2;
    float factor = 1.0 / sqrtf(2.0*M_PI*sigma*sigma);
    for (int x = 0; x <= (size-1)/2; x++) {
        float exp = expf(-1.0*(x*x)/(2.0*sigma*sigma));
        float value = factor*exp;
        set_pixel(filter, center+x, 0, 0, value);
        set_pixel(filter, center-x, 0, 0, value);
    }
    // normalize the weights
    l1_normalize(filter);
    return filter;
}

// Smooths an image using separable Gaussian filter.
// image im: image to smooth.
// float sigma: std dev. for Gaussian.
// returns: smoothed image.
image smooth_image(image im, float sigma)
{
    if(0){
        image g = make_gaussian_filter(sigma);
        image s = convolve_image(im, g, 1);
        free_image(g);
        return s;
    } else {
        // TODONE: optional, use two convolutions with 1d gaussian filter.
        // If you implement, disable the above if check.
        image xfilter = make_1d_gaussian(sigma);
        image yfilter = make_image(1, xfilter.w, 1);
        for (int x = 0; x < xfilter.w; x++) {
            set_pixel(yfilter, 0, x, 0, get_pixel(xfilter, x, 0, 0));
        }
        image xi = convolve_image(im, xfilter, 1);
        image xyi = convolve_image(xi, yfilter, 1);
        // TODO: Remove Debug
        /* image g = make_gaussian_filter(sigma);
        image s = convolve_image(im, g, 1);
        if (xyi.w != s.w || xyi.h != s.h || xyi.c != s.c) {
            printf("fucccccccck************************************");
        }
        for (int x = 0; x < xyi.w; x++) {
            for (int y = 0; y < xyi.h; y++) {
                for (int c = 0; c < xyi.c; c++) {
                    float pix1 = get_pixel(xyi, x, y, c);
                    float pix2 = get_pixel(s, x, y, c);
                    if(pix1 != pix2) {
                        //printf("MISMATCH PIXEL ***********************");
                    }
                }
            }
        } */
        free_image(xfilter);
        free_image(yfilter);
        free_image(xi);
        return xyi;
    }
}

// Calculate the structure matrix of an image.
// image im: the input image.
// float sigma: std dev. to use for weighted sum.
// returns: structure matrix. 1st channel is Ix^2, 2nd channel is Iy^2,
//          third channel is IxIy.
image structure_matrix(image im, float sigma)
{
    // image S = make_image(im.w, im.h, 3);
    // 3.1 TODONE: calculate structure matrix for im.
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();
    image ix = convolve_image(im, gx_filter, 0);
    image iy = convolve_image(im, gy_filter, 0);
    image res = make_image(im.w, im.h, 3);
    for (int x = 0; x < im.w; x++) {
         for (int y = 0; y < im.h; y++) {
             float ix_pix = get_pixel(ix, x, y, 0);
             float iy_pix = get_pixel(iy, x, y, 0);
             set_pixel(res, x, y, 0, ix_pix * ix_pix);
             set_pixel(res, x, y, 1, iy_pix * iy_pix);
             set_pixel(res, x, y, 2, ix_pix * iy_pix);
         }
    }
    image result = smooth_image(res, sigma);
    free_image(gx_filter);
    free_image(gy_filter);
    free_image(ix);
    free_image(iy);
    free_image(res);
    return result;
}

// Estimate the cornerness of each pixel given a structure matrix S.
// image S: structure matrix for an image.
// returns: a response map of cornerness calculations.
image cornerness_response(image S)
{
    image R = make_image(S.w, S.h, 1);
    // 3.2 TODONE: fill in R, "cornerness" for each pixel using the structure matrix.
    // We'll use formulation det(S) - alpha * trace(S)^2, alpha = .06.
    for (int x = 0; x < S.w; x++) {
         for (int y = 0; y < S.h; y++) {
             // the determinant of a 2x2 matrix is A11*A22-A12*A21
             float det = (get_pixel(S, x, y, 0) * get_pixel(S, x, y, 1))
                 - (get_pixel(S, x, y, 2) * get_pixel(S, x, y, 2));
             float alpha = 0.06;
             // the trace of a 2x2 matrix is A11+A22
             float trace = get_pixel(S, x, y, 0) + get_pixel(S, x, y, 1);
             set_pixel(R, x, y, 0, det - (alpha * trace * trace));
         }
    }
    return R;
}

// Perform non-max supression on an image of feature responses.
// image im: 1-channel image of feature responses.
// int w: distance to look for larger responses.
// returns: image with only local-maxima responses within w pixels.
image nms_image(image im, int w)
{
    image r = copy_image(im);
    // 3.3 TODONE: perform NMS on the response map.
    // for every pixel in the image:
    //     for neighbors within w:
    //         if neighbor response greater than pixel response:
    //             set response to be very low (I use -999999 [why not 0??])
    for (int x = 0; x < im.w; x++) {
         for (int y = 0; y < im.h; y++) {
             // for every pixel in the image...
             int notlocalmax = 0;
             for (int nx = 0; nx <= w && notlocalmax==0; nx++) {
                 for (int ny = 0; ny <= w && notlocalmax==0; ny++) {
                     // for every pixel within w pixels (Chebyshev distance)...
                     float value = get_pixel(im, x, y, 0);
                     if(nx ==0 && ny == 0) {
                        continue;
                     }
                     if (get_pixel(im, x-nx, y-ny, 0) >= value) {
                         notlocalmax = 1;
                     } else if (get_pixel(im, x-nx, y+ny, 0) >= value) {
                         notlocalmax = 1;
                     } else if (get_pixel(im, x+nx, y-ny, 0) >= value) {
                         notlocalmax = 1;
                     } else if (get_pixel(im, x+nx, y+ny, 0) >= value) {
                         notlocalmax = 1;
                     }
                 }
             }
             if (notlocalmax==1) {
                 set_pixel(r, x, y, 0, -999999.0f);
             }
         }
    }
    return r;
}

// Perform harris corner detection and extract features from the corners.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
// int *n: pointer to number of corners detected, should fill in.
// returns: array of descriptors of the corners in the image.
descriptor *harris_corner_detector(image im, float sigma, float thresh, int nms, int *n)
{
    // Calculate structure matrix
    image S = structure_matrix(im, sigma);

    // Estimate cornerness
    image R = cornerness_response(S);

    // Run NMS on the responses
    image Rnms = nms_image(R, nms);

    // 3.4 TODONE: count number of responses over threshold
    int count = 0;
    for (int x = 0; x < Rnms.w; x++) {
        for (int y = 0; y < Rnms.h; y++) {
            if (get_pixel(Rnms, x, y, 0) > thresh) {
                count += 1;
            }
        }
    }
    
    *n = count; // <- set *n equal to number of corners in image.
    descriptor *d = calloc(count, sizeof(descriptor));
    // 3.4 TODONE: fill in array *d with descriptors of corners, use describe_index.
    int i = 0;
    for (int x = 0; x < Rnms.w; x++) {
        for (int y = 0; y < Rnms.h; y++) {
            if (get_pixel(Rnms, x, y, 0) > thresh) {
                d[i] = describe_index(im, Rnms.w*y+x);
                i += 1;
            }
        }
    }
 
    free_image(S);
    free_image(R);
    free_image(Rnms);
    return d;
}

// Find and draw corners on an image.
// image im: input image.
// float sigma: std. dev for harris.
// float thresh: threshold for cornerness.
// int nms: distance to look for local-maxes in response map.
void detect_and_draw_corners(image im, float sigma, float thresh, int nms)
{
    int n = 0;
    descriptor *d = harris_corner_detector(im, sigma, thresh, nms, &n);
    mark_corners(im, d, n);
}
