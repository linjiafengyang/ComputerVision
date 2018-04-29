#include "CANNY.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "CImg.h"
using namespace cimg_library;

#define ffabs(x) ( (x) >= 0 ? (x) : -(x) ) 
#define GAUSSIAN_CUT_OFF 0.005f
#define MAGNITUDE_SCALE 100.0f
#define MAGNITUDE_LIMIT 1000.0f
#define MAGNITUDE_MAX ((int) (MAGNITUDE_SCALE * MAGNITUDE_LIMIT))

CImg<unsigned char> CANNY::toGrayScale(CImg<unsigned char> src) {
	int width = src.width();
	int height = src.height();
	int depth = src.depth();

	//New grayscale images.
	CImg<unsigned char> gray(width, height, depth, 1);
	unsigned char r, g, b;
	unsigned char gr1 = 0;

	/* Convert RGB image to grayscale image */
	for (int i = 0; i<width; i++) {
		for (int j = 0; j<height; j++) {
			//Return a pointer to a located pixel value. 
			r = src(i, j, 0, 0); // First channel RED
			g = src(i, j, 0, 1); // Second channel GREEN
			b = src(i, j, 0, 2); // Third channel BLUE
								 //PAL: Y = 0.299*R + 0.587*G + 0.114*B
			gr1 = round(0.299*((double)r) + 0.587*((double)g) + 0.114*((double)b));
			gray(i, j, 0, 0) = gr1;
		}

	}
	return gray;
}

/*
  Canny edge detection with default parameters
    Params: grey - the greyscale image
	        width, height - image width and height
    Returns: binary image with edges as set pixels
*/
unsigned char* CANNY::canny(unsigned char *grey, int width, int height, float lowthreshold, float highthreshold, float gaussiankernelradius, int gaussiankernelwidth)
{
	cannyparam(grey, width, height, lowthreshold, highthreshold, gaussiankernelradius, gaussiankernelwidth, 0);
	return result;
}

/*
 Canny edge detection with parameters passed in by user
   Params: grey - the greyscale image
           width, height - image dimensions
		   lowthreshold - default 2.5
		   highthreshold - default 7.5
		   gaussiankernelradius - radius of edge detection Gaussian, in standard deviations
		     (default 2.0)
		   gaussiankernelwidth - width of Gaussian kernel, in pixels (default 16)
		   contrastnormalised - flag to normalise image before edge detection (defualt 0)
   Returns: binary image with set pixels as edges

*/
void CANNY::cannyparam(unsigned char *grey, int width, int height,
						  float lowthreshold, float highthreshold, 
						  float gaussiankernelradius, int gaussiankernelwidth,  
						  int contrastnormalised)
{
    int low, high;
	int err;
	int i;

	allocatebuffers(grey, width, height);
	if(!this->result)
		goto error_exit;
	if (contrastnormalised) 
		normalizeContrast(data, width, height);

	err = computeGradients(gaussiankernelradius, gaussiankernelwidth);
	if(err < 0)
	  goto error_exit;
    low = (int) (lowthreshold * MAGNITUDE_SCALE + 0.5f);
	high = (int) ( highthreshold * MAGNITUDE_SCALE + 0.5f);
	performHysteresis(low, high);
	for(i=0;i<width*height;i++)
		result[i] = idata[i] > 0 ? 1 : 0;
	
	killbuffers();
	return;
error_exit:
	killbuffers();
	return;
}


/*
  buffer allocation
*/
CANNY* CANNY::allocatebuffers(unsigned char *grey, int width, int height)
{
	if(!this)  
		goto error_exit;
	result = (unsigned char*)malloc(width * height);
	middleChar = (unsigned char*)malloc(width * height);
	middleInt = (int *)malloc(width * height * sizeof(int));
	middleFloat = (float*)malloc(width * height* sizeof(float));

	data = (unsigned char *)malloc(width * height);
	idata = (int *)malloc(width * height * sizeof(int));
	magnitude = (int *)malloc(width * height * sizeof(int));
	xConv = (float *)malloc(width * height * sizeof(float));
	yConv = (float *)malloc(width * height * sizeof(float));
	xGradient = (float *)malloc(width * height * sizeof(float));
	yGradient = (float *)malloc(width * height * sizeof(float));
	if(!data || !idata || !magnitude 
		|| !xConv || !yConv 
		|| !xGradient || !yGradient)
		 goto error_exit;

	memcpy(data, grey, width * height);
	this->width = width;
	this->height = height;

	return this;
error_exit:
	killbuffers();
	return 0;
}

/*
  buffers destructor
*/
void CANNY::killbuffers()
{
	if(this)
	{
		free(data);
		free(idata);
		free(magnitude);
		free(xConv);
		free(yConv);
		free(xGradient);
		free(yGradient);
	}
}

/* NOTE: The elements of the method below (specifically the technique for
	non-maximal suppression and the technique for gradient computation)
	are derived from an implementation posted in the following forum (with the
	clear intent of others using the code):
	  http://forum.java.sun.com/thread.jspa?threadID=546211&start=45&tstart=0
	My code effectively mimics the algorithm exhibited above.
	Since I don't know the providence of the code that was posted it is a
	possibility (though I think a very remote one) that this code violates
	someone's intellectual property rights. If this concerns you feel free to
	contact me for an alternative, though less efficient, implementation.
	*/
	
int CANNY::computeGradients(float kernelRadius, int kernelWidth)
{	
		float *kernel;
		float *diffKernel;
		int kwidth;

		int initX;
		int maxX;
		int initY;
		int maxY;

		int x, y;
		int i;
		int flag;

		kernel = (float *)malloc(kernelWidth * sizeof(float));
		diffKernel = (float *)malloc(kernelWidth * sizeof(float));
		if(!kernel || !diffKernel)
			goto error_exit;

		/* initialise the Gaussian kernel */
		for (kwidth = 0; kwidth < kernelWidth; kwidth++) 
		{
			float g1, g2, g3;
			g1 = gaussian((float) kwidth, kernelRadius);
			if (g1 <= GAUSSIAN_CUT_OFF && kwidth >= 2) 
				break;
			g2 = gaussian(kwidth - 0.5f, kernelRadius);
			g3 = gaussian(kwidth + 0.5f, kernelRadius);
			kernel[kwidth] = (g1 + g2 + g3) / 3.0f / (2.0f * (float) 3.14 * kernelRadius * kernelRadius);
			diffKernel[kwidth] = g3 - g2;
		}

		initX = kwidth - 1;
		maxX = width - (kwidth - 1);
		initY = width * (kwidth - 1);
		maxY = width * (height - (kwidth - 1));
		
		/* perform convolution in x and y directions */
		for(x = initX; x < maxX; x++) 
		{
			for(y = initY; y < maxY; y += width) 
			{
				int index = x + y;
				float sumX = data[index] * kernel[0];
				float sumY = sumX;
				int xOffset = 1;
				int yOffset = width;
				while(xOffset < kwidth) 
				{
					sumY += kernel[xOffset] * (data[index - yOffset] + data[index + yOffset]);
					sumX += kernel[xOffset] * (data[index - xOffset] + data[index + xOffset]);
					yOffset += width;
					xOffset++;
				}
				
				yConv[index] = sumY;
				xConv[index] = sumX;
			}
 
		}

		for (int index = 0; index<width*height; index++) //在高斯滤波图
			middleFloat[index] = hypotenuse(xConv[index], yConv[index]);
 
		for (x = initX; x < maxX; x++) 
		{
			for (y = initY; y < maxY; y += width) 
			{
				float sum = 0.0f;
				int index = x + y;
				for (i = 1; i < kwidth; i++)
					sum += diffKernel[i] * (yConv[index - i] - yConv[index + i]);
 
				xGradient[index] = sum;
			}
 
		}

		for(x = kwidth; x < width - kwidth; x++) 
		{
			for (y = initY; y < maxY; y += width) 
			{
				float sum = 0.0f;
				int index = x + y;
				int yOffset = width;
				for (i = 1; i < kwidth; i++) 
				{
					sum += diffKernel[i] * (xConv[index - yOffset] - xConv[index + yOffset]);
					yOffset += width;
				}
 
				yGradient[index] = sum;
			}
		}
		//for (int index = 0; index<width*height; index++) //梯度模图像
		//	middleFloat[index] = xGradient[index] + yGradient[index];
		//	middleFloat[index] = yGradient[index];  
		//	middleFloat[index] = hypotenuse(can->xGradient[index], can->yGradient[index]); // 不对 
 
		initX = kwidth;
		maxX = width - kwidth;
		initY = width * kwidth;
		maxY = width * (height - kwidth);
		for(x = initX; x < maxX; x++) 
		{
			for(y = initY; y < maxY; y += width) 
			{
				int index = x + y;
				int indexN = index - width;
				int indexS = index + width;
				int indexW = index - 1;
				int indexE = index + 1;
				int indexNW = indexN - 1;
				int indexNE = indexN + 1;
				int indexSW = indexS - 1;
				int indexSE = indexS + 1;
				
				float xGrad = xGradient[index];
				float yGrad = yGradient[index];
				float gradMag = hypotenuse(xGrad, yGrad);

				/* perform non-maximal supression */
				float nMag = hypotenuse(xGradient[indexN], yGradient[indexN]);
				float sMag = hypotenuse(xGradient[indexS], yGradient[indexS]);
				float wMag = hypotenuse(xGradient[indexW], yGradient[indexW]);
				float eMag = hypotenuse(xGradient[indexE], yGradient[indexE]);
				float neMag = hypotenuse(xGradient[indexNE], yGradient[indexNE]);
				float seMag = hypotenuse(xGradient[indexSE], yGradient[indexSE]);
				float swMag = hypotenuse(xGradient[indexSW], yGradient[indexSW]);
				float nwMag = hypotenuse(xGradient[indexNW], yGradient[indexNW]);
				float tmp;
				/*
				 * An explanation of what's happening here, for those who want
				 * to understand the source: This performs the "non-maximal
				 * supression" phase of the Canny edge detection in which we
				 * need to compare the gradient magnitude to that in the
				 * direction of the gradient; only if the value is a local
				 * maximum do we consider the point as an edge candidate.
				 * 
				 * We need to break the comparison into a number of different
				 * cases depending on the gradient direction so that the
				 * appropriate values can be used. To avoid computing the
				 * gradient direction, we use two simple comparisons: first we
				 * check that the partial derivatives have the same sign (1)
				 * and then we check which is larger (2). As a consequence, we
				 * have reduced the problem to one of four identical cases that
				 * each test the central gradient magnitude against the values at
				 * two points with 'identical support'; what this means is that
				 * the geometry required to accurately interpolate the magnitude
				 * of gradient function at those points has an identical
				 * geometry (upto right-angled-rotation/reflection).
				 * 
				 * When comparing the central gradient to the two interpolated
				 * values, we avoid performing any divisions by multiplying both
				 * sides of each inequality by the greater of the two partial
				 * derivatives. The common comparand is stored in a temporary
				 * variable (3) and reused in the mirror case (4).
				 * 
				 */
				flag = ( (xGrad * yGrad <= 0.0f) /*(1)*/
					? ffabs(xGrad) >= ffabs(yGrad) /*(2)*/
						? (tmp = ffabs(xGrad * gradMag)) >= ffabs(yGrad * neMag - (xGrad + yGrad) * eMag) /*(3)*/
							&& tmp > fabs(yGrad * swMag - (xGrad + yGrad) * wMag) /*(4)*/
						: (tmp = ffabs(yGrad * gradMag)) >= ffabs(xGrad * neMag - (yGrad + xGrad) * nMag) /*(3)*/
							&& tmp > ffabs(xGrad * swMag - (yGrad + xGrad) * sMag) /*(4)*/
					: ffabs(xGrad) >= ffabs(yGrad) /*(2)*/
						? (tmp = ffabs(xGrad * gradMag)) >= ffabs(yGrad * seMag + (xGrad - yGrad) * eMag) /*(3)*/
							&& tmp > ffabs(yGrad * nwMag + (xGrad - yGrad) * wMag) /*(4)*/
						: (tmp = ffabs(yGrad * gradMag)) >= ffabs(xGrad * seMag + (yGrad - xGrad) * sMag) /*(3)*/
							&& tmp > ffabs(xGrad * nwMag + (yGrad - xGrad) * nMag) /*(4)*/
					);
                if(flag)
				{
					magnitude[index] = (gradMag >= MAGNITUDE_LIMIT) ? MAGNITUDE_MAX : (int) (MAGNITUDE_SCALE * gradMag);
					/*NOTE: The orientation of the edge is not employed by this
					 implementation. It is a simple matter to compute it at
					this point as: Math.atan2(yGrad, xGrad); */
				} 
				else 
				{
					magnitude[index] = 0;
				}
			}
		}
		for (int index = 0; index<width*height; index++) //极大值抑制
			middleInt[index] = magnitude[index];

		free(kernel);
		free(diffKernel);
		return 0;
error_exit:
		free(kernel);
		free(diffKernel);
		return -1;
}
	
	/*
	  we follow edges. high gives the parameter for starting an edge,
	  how the parameter for continuing it.
	*/
void CANNY::performHysteresis(int low, int high)
{
  int offset = 0;
  int x, y;
	
  memset(idata, 0, width * height * sizeof(int));
		
  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
	{
      if(idata[offset] == 0 && magnitude[offset] >= high) //!!!!
	    follow(x, y, offset, low);
	  offset++;
    }
  }
}
 
/*
	recursive portion of edge follower 
*/
void CANNY::follow(int x1, int y1, int i1, int threshold)
{
  int x, y;
  int x0 = x1 == 0 ? x1 : x1 - 1;
  int x2 = x1 == width - 1 ? x1 : x1 + 1;
  int y0 = y1 == 0 ? y1 : y1 - 1;
  int y2 = y1 == height -1 ? y1 : y1 + 1;
		
  idata[i1] = magnitude[i1];  // !!!
  for (x = x0; x <= x2; x++) 
  {
    for (y = y0; y <= y2; y++) 
	{
      int i2 = x + y * width;
	  if ((y != y1 || x != x1) && idata[i2] == 0 && magnitude[i2] >= threshold)  // !!!
	    follow(x, y, i2, threshold);
    }
  }
}

void CANNY::normalizeContrast(unsigned char *data, int width, int height)
{
	int histogram[256] = {0};
    int remap[256];
	int sum = 0;
    int j = 0;
	int k;
    int target;
    int i;

	for (i = 0; i < width * height; i++) 
			histogram[data[i]]++;
		
	
    for (i = 0; i < 256; i++) 
	{
			sum += histogram[i];
			target = (sum*255)/(width * height);
			for (k = j+1; k <= target; k++) 
				remap[k] = i;
			j = target;
	 }
		
    for (i = 0; i < width * height; i++) 
			data[i] = remap[data[i]];
}

float CANNY::hypotenuse(float x, float y)
{
	return (float) sqrt(x*x +y*y);
}
 
float CANNY::gaussian(float x, float tempSigma)
{
	return (float) exp(-(x * x) / (2.0f * tempSigma * tempSigma));
}
