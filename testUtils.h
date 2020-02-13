#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <hls_opencv.h>
#include "core.h"
#include <iomanip>

void saveImage(const std::string path, cv::InputArray inArr);
void printSmallMatrixCVShort(char *title, cv::InputArray inArr);
void printSmallMatrixCVChar(char *title, cv::InputArray inArr);
void conv2dByHand(cv::InputArray imageIn, float imageOut[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS],  float kernel[KERNEL_DIM_conv1][KERNEL_DIM_conv1][KERNEL_DIM_conv1], int kSize);



short processWindowAndKernel(float kernel[KERNEL_DIM_conv1][KERNEL_DIM_conv1][KERNEL_DIM_conv1], short window[KERNEL_DIM_conv1][KERNEL_DIM_conv1][KERNEL_DIM_conv1], int dim);
