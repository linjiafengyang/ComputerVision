#include "CImg.h"
#include <iostream>
#include <vector>
using namespace cimg_library;
using namespace std;

struct Point {
	int x, y, cnt;
	Point(int _x, int _y, int _cnt): x(_x), y(_y), cnt(_cnt) {}
};

struct Line {
	double k, b;
	Line(double _k, double _b): k(_k), b(_b) {}
};
class Hough {
private:
	CImg<float> src; // 输入的原图
	CImg<float> blurred_img; // 高斯滤波平滑得到的图
	CImg<float> houghspace; // 霍夫空间图
	CImg<float> result; // 最后得到的结果图
	vector<Point> peaks; // 霍夫空间直线经过最多的点
	vector<Line> lines; // 直线
	vector<Point> intersections; // 直线交点
	double sigma;
	double gradient_threshold;
	double vote_threshold;
	double peak_dis;
	int x_min, x_max, y_min, y_max;
public:
	Hough(CImg<float> srcImg, double sigma, double gradient_threshold, double vote_threshold, double peak_dis);
	CImg<float> houghProcess(CImg<float> srcImg);
	CImg<float> RGBtoGray(const CImg<float>& srcImg); // 转灰度图
	CImg<float> initHoughSpace(); // 初始化霍夫空间
	void findPeaks(); // 投票算法
	void drawLines(); // 寻找并画出直线
	void drawIntersections(); // 寻找并画出直线交点
};