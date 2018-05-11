#include <iostream>
#include "CImg.h"
#include <vector>
#include <string>

using namespace std;
using namespace cimg_library;

struct Point {
	float x, y;
	Point(float _x, float _y): x(_x), y(_y) {}
};
struct Triangle {
	Point p1, p2, p3;
	Triangle(Point _p1, Point _p2, Point _p3): p1(_p1), p2(_p2), p3(_p3) {}
};
class ImageMorphing {
private:
	vector<Point> src_points;
	vector<Point> dst_points;
	vector<vector<Point> > mid_points;

	vector<vector<int> > index;
	vector<Triangle> src_triangle_list;
	vector<Triangle> dst_triangle_list;
	vector<vector<Triangle> > mid_triangle_list;

	CImg<float> src;
	CImg<float> dst;
	CImgList<float> result;

	int frame_cnt;
public:
	ImageMorphing(CImg<float> src, CImg<float> dst, vector<Point> src_points, vector<Point> dst_points, 
					vector<vector<int> > index, int frame_cnt);
	void setSrcTriangleList();
	void setDstTriangleList();
	void setMidTriangleList();
	CImg<float> computeTransformMatrix(Triangle before, Triangle after); // 计算变换矩阵
	bool isInTriangle(Point P, Triangle tri); // 重心法判断一个点是否在三角形内
	CImgList<float> morphing();
};