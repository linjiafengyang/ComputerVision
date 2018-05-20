#include "CImg.h"
#include <iostream>
using namespace cimg_library;
using namespace std;

class OSTU {
public:
	CImg<float> run_ostu(const CImg<float>& image);
private:
	int ostu(const CImg<float>& image);
	CImg<float> RGBtoGray(const CImg<float>& image);
	CImg<float> segment(CImg<float> image, int threshold);
	int threshold; // 阈值
	double variance; // 类间方差
	double P1; // 前景像素点数所占比例
	double P2; // 背景像素点数所占比例
	double m1; // 前景像素平均灰度
	double m2; // 背景像素平均灰度
	double histogram[256]; // 灰度直方图
	double pixelsNum; // 像素点总数
};