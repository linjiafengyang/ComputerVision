#include "CImg.h"
#include <iostream>
#include <vector>
using namespace cimg_library;
using namespace std;

struct Point {
	double x, y;
	int cnt;
	Point(double _x, double _y, int _cnt): x(_x), y(_y), cnt(_cnt) {}
};
struct Line {
	double k, b;
	Line(double _k, double _b): k(_k), b(_b) {}
	int index;
	double distance;
	Line(int _index, double _distance): index(_index), distance(_distance) {}
};
class Hough {
private:
	CImg<float> grayImage; // 灰度图
	CImg<float> blurred_img; // 高斯滤波平滑得到的图
	CImg<float> houghspace; // 霍夫空间图
	CImg<float> hough_result; // 霍夫检测图
	CImg<float> A4; // a4纸结果图
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
	CImg<float> imageWarping(CImg<float> srcImg);
	CImg<float> RGBtoGray(const CImg<float>& srcImg); // 转灰度图
	CImg<float> initHoughSpace(); // 初始化霍夫空间
	void findPeaks(); // 投票算法
	void drawLines(); // 寻找并画出直线
	void drawIntersections(); // 寻找并画出直线交点
	vector<CImg<float> > computeTransformMatrix(CImg<float> a4); // 计算变换矩阵
	CImg<float> warping(CImg<float> srcImg); // image warping
};