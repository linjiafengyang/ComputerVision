#ifndef canny_h
#define canny_h

#include "CImg.h"
using namespace cimg_library;

class Canny
{
public:
	CImg<unsigned char> canny(CImg<unsigned char> grey, int width, int height);
	CImg<unsigned char> cannyparam(CImg<unsigned char> grey, int width, int height,
						  float lowthreshold, float highthreshold,
						  float gaussiankernelradius, int gaussiankernelwidth,  
						  int contrastnormalised);

	Canny *allocatebuffers(const CImg<unsigned char> & grey, int width, int height);
	void killbuffers(Canny *can);
	int computeGradients(Canny *can, float kernelRadius, int kernelWidth);
	void performHysteresis(Canny *can, int low, int high);
	void follow(Canny *can, int x1, int y1, int i1, int threshold);
	void normalizeContrast(CImg<unsigned char> & data, int width, int height);
	float hypotenuse(float x, float y);
	float gaussian(float x, float sigma);

private:
	CImg<unsigned char> data; /* input image */
	int width;
	int height;
	int *idata;          /* output for edges */
	int *magnitude;      /* edge magnitude as detected by Gaussians */
	float *xConv;        /* temporary for convolution in x direction */
	float *yConv;        /* temporary for convolution in y direction */
	float *xGradient;    /* gradients in x direction, as detected by Gaussians */
	float *yGradient;    /* gradients in x direction,a s detected by Gaussians */
	
};

#endif


// #ifndef canny_h
// #define canny_h

// unsigned char *canny(unsigned char *grey, int width, int height);
// unsigned char *cannyparam(unsigned char *grey, int width, int height, 
// 						  float lowThreshold, float highthreshold, 
// 						  float gaussiankernelradius, int gaussiankernelwidth,  
// 						  int contrastnormalised);
// #endif