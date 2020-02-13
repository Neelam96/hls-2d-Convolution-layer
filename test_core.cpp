/*
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <hls_opencv.h>
#include "core.h"

#define blue_image "blue_image.jpg"
#define red_image "red_image.jpg"
#define green_image "green_image.jpg"

//edge detection
char kernel_11[KERNEL_DIM_conv1*KERNEL_DIM_conv1] = {
		-1,-1,-1,
		-1,8,-1,
		-1,-1,-1
};

char kernel_12[KERNEL_DIM_conv1*KERNEL_DIM_conv1] = {
		-1,-1,-1,
		-1,8,-1,
		-1,-1,-1
};

char kernel_13[KERNEL_DIM_conv1*KERNEL_DIM_conv1] = {
		-1,-1,-1,
		-1,8,-1,
		-1,-1,-1
};

char outImage[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS];
char outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS];



//void referenceFunction(int imageIn[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS],int kernel[KERNEL_DIM][KERNEL_DIM], int imageOut[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS] )
//{
//	for(int y = 0;y < IMG_HEIGHT_OR_ROWS ; y++){
//		for(int x=0 ; x < IMG_WIDTH_OR_COLS; x++)
//		{
//			if(x==0||y==0|| x==IMG_WIDTH_OR_COLS-1||y==IMG_HEIGHT_OR_ROWS-1){
//				imageOut[y][x] = 0;
//			}
//			else{
//				//compute imageOut[y][x]
//				imageOut[y][x] = 0;
//				for(int i=0;i<3;i++){
//					for(int j=0;j<3;j++){
//						imageOut[y][x] = imageOut[y][x] + (kernel[i][j]*imageIn[i+y-1][j+x-1]);
//					}
//				}
//			}
//		}
//	}
//}


//
//void print_kernel(){
//	for(int i=0;i<KERNEL_DIM;i++){
//		for(int j=0;j<KERNEL_DIM;j++)
//		{
//			short val = (short) kernel[i*KERNEL_DIM+j];
//			printf("%hd ",val);
//		}
//		printf("\n");
//	}
//}

void print_image_matrix(char* img,int H,int W)
{
	printf("%s\n",img);
	cv::Mat src = cv::imread(img, CV_LOAD_IMAGE_COLOR);
	for(int i=0;i<H;i++)
		{
			for(int j=0;j<W;j++)
			{
				uint8_t val = src.at<uint8_t>(i,j);
				printf("%hhu\t",val);
			}
			printf("\n");
		}

}

int main()
{
	cv::Mat src = cv::imread(INPUT_IMAGE, CV_LOAD_IMAGE_COLOR);

	cv::Mat bgr[3];
	cv::split(src,bgr);

	cv::imwrite(blue_image, bgr[0]);
	cv::imwrite(green_image, bgr[1]);
	cv::imwrite(red_image,bgr[2]);

//	for(int i=0;i<IMG_HEIGHT_OR_ROWS;i++)
//	{
//		for(int j=0;j<IMG_WIDTH_OR_COLS;j++)
//		{
//			uint8_t val = src.at<uint8_t>(i,j);
//			printf("%hhu\t",val);
//		}
//		printf("\n");
//	}
	print_image_matrix(blue_image,IMG_HEIGHT_OR_ROWS,IMG_WIDTH_OR_COLS);

	return 0;

}

*/

///////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <hls_opencv.h>

#include "testUtils.h"
#include <fstream>


// Edge
float kernel[KERNEL_DIM_conv1][KERNEL_DIM_conv1][KERNEL_DIM_conv1] = {
		{{-0.03881463,  0.02160241,  0.09580971},
		{-0.03881463,  0.02160241,  0.09580971},
		{-0.03881463,  0.02160241,  0.09580971}},

		{{-0.03881463,  0.02160241,  0.09580971},
		{-0.03881463,  0.02160241,  0.09580971},
		{-0.03881463,  0.02160241,  0.09580971}},

		{{-0.03881463,  0.02160241,  0.09580971},
		{-0.03881463,  0.02160241,  0.09580971},
		{-0.03881463,  0.02160241,  0.09580971}}
};



// Image File path
float outImage[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS];
float outImageRef[IMG_HEIGHT_OR_ROWS][IMG_WIDTH_OR_COLS];

int main()
{
	// Read input image
	std::ofstream myfile;
	myfile.open("Outputs_log.txt");

	printf("Load image %s\n",INPUT_IMAGE);
	cv::Mat imageSrc;
	imageSrc = cv::imread(INPUT_IMAGE, CV_LOAD_IMAGE_COLOR);

	// Convert to grayscale
	//cv::cvtColor(imageSrc, imageSrc, CV_BGR2GRAY);
	//printf("Image Rows:%d Cols:%d\n",imageSrc.rows, imageSrc.cols);

	// Define streams for input and output
	hls::stream<uint_8> inputStream;
	hls::stream<my_data> outputStream;

	// OpenCV mat that point to a array (cv::Size(Width, Height))
	cv::Mat imgCvOut(cv::Size(imageSrc.cols, imageSrc.rows), CV_8UC1, outImage, cv::Mat::AUTO_STEP);
	cv::Mat imgCvOutRef(cv::Size(imageSrc.cols, imageSrc.rows), CV_8UC1, outImageRef, cv::Mat::AUTO_STEP);

	// Populate the input stream with the image bytes
	myfile<<"Printing image pixel dimensions <row,col,dim,val>\n";
	for (int idxRows=0; idxRows < imageSrc.rows; idxRows++)
	{
		for (int idxCols=0; idxCols < imageSrc.cols; idxCols++)
		{
			for (int dim = 0; dim < KERNEL_DIM_conv1; dim++)
			{

				uint_8 valIn = imageSrc.at<cv::Vec3b>(idxRows,idxCols)[dim];
//				valIn.last = (idxRows*idxCols*dim == (imageSrc.rows*imageSrc.cols*KERNEL_DIM_conv1)-1?1:0);
				short data = valIn;
				printf("%lu ",data);

				inputStream << valIn;
//				inputStream << valIn.val[1];
//				inputStream << valIn.val[2];
//				printf("%u: ",valIn.val[0],"%u: ",valIn.val[1],"%u: \n",valIn.val[2]);
//				cout << static_cast<int>(imageSrc.at<cv::Vec3b>(idxRows,idxCols)[0] );
				myfile<<"row: "<<idxRows<<" col: "<<idxCols<<" dim: "<<dim<<" val: "<<valIn<<"\n";


			}

		}
	}
	myfile<<"---------------------------------------------------------\n";

	// Do the convolution (Reference)
	printf("Calling Reference function\n");
	conv2dByHand(imageSrc,outImageRef,kernel,KERNEL_DIM_conv1);
	printf("Reference function ended\n");

	for(int i=0;i<CONV1_OUT_H;i++)
	{
		for(int j=0;j<CONV1_OUT_W;j++)
		{
//			printf("%f ",outImageRef[i][j]);
		}
//		printf("\n");
	}

	// Save image out file or display

//		printSmallMatrixCVChar("Ref Core", imgCvOutRef);

		printf("Saving image Ref\n");
		saveImage(std::string(OUTPUT_IMAGE) ,imgCvOutRef);



	// Do the convolution on the core (Third parameter choose operation 0(conv),1(erode),2(dilate)
	printf("Calling Core function\n");
	convolve_2d(inputStream, outputStream, kernel, KERNEL_DIM_conv1, IMG_HEIGHT_OR_ROWS, IMG_WIDTH_OR_COLS, IMG_CHANNELS);
	printf("Core function ended\n");

	// Take data from the output stream to our array outImage (Pointed in opencv)
		for (int idxRows=0; idxRows < imageSrc.rows-KERNEL_DIM_conv1+1; idxRows++)
			{
				for (int idxCols=0; idxCols < imageSrc.cols-KERNEL_DIM_conv1+1; idxCols++)
				{
					my_data valOut;
					outputStream.read(valOut);
					outImage[idxRows][idxCols] = valOut.data;

//					printf("%f: \n",outImageRef[idxRows][idxCols]);
//					printf("%f: \n",valOut.data);
					if(outImage[idxRows][idxCols]!=outImageRef[idxRows][idxCols]){
//						printf("Wrong output at row: %d",idxRows," col: %d",idxCols,"\n");
//						printf("outImage: %f",outImage[idxRows][idxCols],"Ref: %f \n",outImageRef[idxRows][idxCols]);
						//myfile<<"outImage: "<<outImage[idxRows][idxCols]<<" Ref: "<<outImageRef[idxRows][idxCols]<<"\n";
					}
//					printf("outImage: %f",outImage[idxRows][idxCols],"Ref: %f \n",outImageRef[idxRows][idxCols]);
					myfile<<"outImage: "<<std::fixed<<std::setprecision(5)<<outImage[idxRows][idxCols]<<" Ref: "<<std::fixed<<std::setprecision(5)<<outImageRef[idxRows][idxCols]<<"\n";
				}
			}



		myfile.close();
//		printSmallMatrixCVChar("Res Core", imgCvOut);
		printf("Saving image\n");
		saveImage(std::string(OUTPUT_IMAGE) ,imgCvOut);

	return 0;

}
