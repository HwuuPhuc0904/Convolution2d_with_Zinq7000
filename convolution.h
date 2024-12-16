#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include <assert.h>
#include <stdint.h>
#include <hls_stream.h>

#define MAX_IMG_ROWS 1080
#define MAX_IMG_COLS 1920

#define TEST_IMG_ROWS 135
#define TEST_IMG_COLS 240
#define TEST_IMG_SIZE (TEST_IMG_ROWS * TEST_IMG_COLS)

typedef uint32_t data_t;

void filter11x11_orig(
        int w, int h,
        const data_t *src_image, data_t *dst_image);

void filter11x11_strm(
        int w, int h,
        hls::stream<data_t> &src_image, hls::stream<data_t> &dst_image);

#endif

