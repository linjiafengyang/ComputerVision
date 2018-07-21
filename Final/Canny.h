#ifndef CANNY_H
#define CANNY_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

const int gFilterx = 5;
const int gFiltery = 5;
const float sigma = 1;
const int threshold_min = 100;
const int threshold_max = 120;

class Canny {
private:
	vector<vector<float> > filter;
public:
	CImg<float> run(CImg<float> grayImage);
	vector<vector<float> > createFilter(int row, int column, float sigmaIn);
	CImg<float> useFilter(CImg<float>& img_in, vector<vector<float> >& filterIn);
	CImg<float> sobel(CImg<float>& gFiltered, CImg<float>& angles);
	CImg<float> nonMaxSupp(CImg<float>& sFiltered, CImg<float> & angles);
	CImg<float> threshold(CImg<float>& imgin, int low, int high);
};

#endif