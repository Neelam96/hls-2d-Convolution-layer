#include "core.h"

void convolve_2d(hls::stream<uint_8> &inStream, hls::stream<my_data> &outStream,float kernel[IMG_CHANNELS][KERNEL_DIM_conv1][KERNEL_DIM_conv1],int KERNEL_DIM, int H, int W, int C)
{
	#pragma HLS INTERFACE axis port=inStream
	#pragma HLS INTERFACE axis port=outStream

//#pragma HLS INTERFACE s_axilite port= return bundle=CRTL_BUS
	#pragma HLS INTERFACE s_axilite port=kernel bundle=KERNEL_BUS

#ifndef __SYNTHESIS__

	hls::LineBuffer<KERNEL_DIM_conv1,IMG_WIDTH_OR_COLS,unsigned char> lineBuff[IMG_CHANNELS];		//only possible if original image is streamed, because unsigned char is taken
	hls::Window<KERNEL_DIM_conv1,KERNEL_DIM_conv1,float> window[IMG_CHANNELS];

	int idxRow = 0;
	int idxCol = 0;
	uint8_t dim = 0; //dim = 0 (red), dim = 1 (blue), dim=2 (green)


	int pix_to_be_convolved = 0;		//gives the col no of the window from where to start filling the window
	int countWait = 0;
	int waitTicks = ((IMG_WIDTH_OR_COLS*(KERNEL_DIM-1)+KERNEL_DIM)*3)/2;
	int idPix=0;

	for(int i=0; i<H*W*C; i++)
	{
		#pragma HLS PIPELINE

//		my_data inStreamData = inStream.read();
		unsigned char pixelIn = (unsigned char) inStream.read();

		lineBuff[dim].shift_up(idxCol);
		lineBuff[dim].insert_top(pixelIn, idxCol);

		//inserting values into the window
		for(int idxWinRow = 0; idxWinRow < KERNEL_DIM; idxWinRow++)
		{
			for(int idxWinCol = 0; idxWinCol < KERNEL_DIM; idxWinCol++)
			{

				int output_pixel_offset = (pix_to_be_convolved)%(W-KERNEL_DIM+1);
				float val = (float) lineBuff[dim].getval(idxWinRow, idxWinCol+output_pixel_offset);
//				val = (float) (kernel[dim][idxWinRow][idxWinCol]*val);
				window[dim].insert(val,idxWinRow,idxWinCol);
			}
		}

		my_data oPixel;
		oPixel.data =0;
		oPixel.last=0;

		if(idxRow>=(KERNEL_DIM-1) && idxCol>=(KERNEL_DIM -1))
		{
			if(dim==C-1)
			{
				//operation can be started here for multiplication and add
				 multWindow(&window[0], kernel,0);
				 multWindow(&window[1], kernel,1);
				 multWindow(&window[2], kernel,2);

				oPixel.data = sumWindow(window[0]);
				oPixel.data = (float) (oPixel.data + sumWindow(window[1]));
				oPixel.data =(float) (oPixel.data + sumWindow(window[2]));
//				printf("%f: \n",oPixel.data);
				pix_to_be_convolved++;
			}
		}
		else{
			oPixel.data =0;
		}

		countWait++;
		if ((countWait > waitTicks) && (dim == C-1))
				{
//					oPixel.last = (i==(H*W*C)-1)?1:0;
					outStream.write(oPixel);
					// Calculate pixel number of an image
					if (idPix<W*H*C)
					{
						idPix++;
					} else {
						idPix = 0;
					}
				}


				//Calculate row and column index
				if (dim == C - 1)
				{
					if (idxCol < W-1)
					{
						idxCol++;
					} else {
						idxCol = 0;
						idxRow++;
					}
					dim = 0;
				} else {
					dim++;
				}

			}

	my_data result;
	for (countWait = 0; countWait < waitTicks; countWait++)
		{
			result.data = 0;
			result.last = countWait==waitTicks-1?1:0;
			outStream.write(result);
		}


#endif
	}



float sumWindow(hls::Window<KERNEL_DIM_conv1,KERNEL_DIM_conv1,float> window)
{
	float result=0;

	for(int r=0; r<KERNEL_DIM_conv1; r++)
		{
			for(int c=0;c<KERNEL_DIM_conv1;c++)
			{
				result+= (float) window.getval(r,c);
			}
		}


//	printf("%f: ",result);


	return result;

}

void multWindow(hls::Window<KERNEL_DIM_conv1,KERNEL_DIM_conv1,float> *window, float kernel[IMG_CHANNELS][KERNEL_DIM_conv1][KERNEL_DIM_conv1],int dim)
{

	float val=1;

		for(int i=0;i<KERNEL_DIM_conv1;i++)
		{
			for(int j=0; j<KERNEL_DIM_conv1;j++)
			{
				val = (float) (window->getval(i,j)*kernel[dim][i][j]);
				window->insert(val,i,j);
			}
		}


}
