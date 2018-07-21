#ifndef HOUGH_H
#define HOUGH_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

const int theta_size = 500;
const int point_num = 4;

class Hough {
private:
	CImg<float> thres;
	CImg<float> houghspace;
	CImg<float> edgeImage;
	vector<pair<int, int> > lines;
	vector<pair<int, int> > edge;
	vector<pair<int, int> > corner;
	vector<float> tabSin;
	vector<float> tabCos;
private:
	void houghLinesTransform(CImg<float> &imgin);
	void houghLinesDetect();
	int getMaxValue(CImg<float> &img, int &size, int &y,int &x);
	void findEdge();
	void findPoint(int scale);
public:
	CImg<float> run(CImg<float> thres, int scale);
	vector<pair<int, int> > getCorner();
};

#endif