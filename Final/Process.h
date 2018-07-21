#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

class Process {
private:
	int scale;
	CImg<float> image;
public:
	CImg<float> RGBtoGray(const CImg<float>& srcImg);
	CImg<float> preprocess(string filename);
	int getScale();
};

#endif