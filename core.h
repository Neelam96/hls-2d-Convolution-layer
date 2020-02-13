#include "hls_video.h"

#include<ap_int.h>
#include<stdint.h>		//includes uint8_t

#define IMG_WIDTH_OR_COLS 320
#define IMG_HEIGHT_OR_ROWS 240
#define IMG_CHANNELS 3

#define word_length 32
#define int_length 23

//3X3 kernel
#define KERNEL_DIM_conv1 3

#define CONV1_OUT_W IMG_WIDTH_OR_COLS-2
#define CONV1_OUT_H IMG_HEIGHT_OR_ROWS-2

#define INPUT_IMAGE	"Lena_input.bmp"
#define OUTPUT_IMAGE	"Lena_output.bmp"

typedef ap_uint<8> uint_8;
typedef ap_fixed<word_length,int_length> fix_32_23;

struct my_data{
	float data;
	bool last;
};

float sumWindow(hls::Window<KERNEL_DIM_conv1,KERNEL_DIM_conv1,float> window);
void convolve_2d(hls::stream<uint_8> &inStream, hls::stream<my_data> &outStream,float kernel[IMG_CHANNELS][KERNEL_DIM_conv1][KERNEL_DIM_conv1],int KERNEL_DIM, int H, int W, int C);
void multWindow(hls::Window<KERNEL_DIM_conv1,KERNEL_DIM_conv1,float> *window, float kernel[IMG_CHANNELS][KERNEL_DIM_conv1][KERNEL_DIM_conv1],int dim);
