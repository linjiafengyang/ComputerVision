#include "CImg.h"
#include <math.h>
#include <iostream>
using namespace cimg_library;
using namespace std;

class Iteration {
public:
	CImg<float> run_iteration(const CImg<float>& image);
private:
	int iteration(const CImg<float>& image);
	CImg<float> RGBtoGray(const CImg<float>& image);
	CImg<float> segment(CImg<float> image, int threshold);
	int threshold; // 阈值
	double histogram[256]; // 灰度直方图
	double pixelsNum; // 像素点总数
};