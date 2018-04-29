#ifndef CANNY_H
#define CANNY_H

#include "CImg.h"
using namespace cimg_library;

class CANNY {
private:
	unsigned char *data; /* input image */
	int width;
	int height;
	int *idata;          /* output for edges */
	int *magnitude;      /* edge magnitude as detected by Gaussians */
	float *xConv;        /* temporary for convolution in x direction */
	float *yConv;        /* temporary for convolution in y direction */
	float *xGradient;    /* gradients in x direction, as detected by Gaussians */
	float *yGradient;    /* gradients in x direction,a s detected by Gaussians */

	void cannyparam(unsigned char *grey, int width, int height,
		float lowThreshold, float highthreshold,
		float gaussiankernelradius, int gaussiankernelwidth,
		int contrastnormalised);
	CANNY *allocatebuffers(unsigned char *grey, int width, int height);
	void killbuffers();
	int computeGradients(float kernelRadius, int kernelWidth);
	void performHysteresis(int low, int high);
	void follow(int x1, int y1, int i1, int threshold);

	void normalizeContrast(unsigned char *data, int width, int height);
	float hypotenuse(float x, float y);
	float gaussian(float x, float sigma);

public:
	unsigned char* result;
	unsigned char* middleChar;
	int* middleInt;
	float* middleFloat;

	CImg<unsigned char> toGrayScale(CImg<unsigned char> src);
	unsigned char *canny(unsigned char *grey, int width, int height, float lowthreshold, float highthreshold, float gaussiankernelradius, int gaussiankernelwidth);
};

#endif