#ifndef IMAGEWARPING_H
#define IMAGEWARPING_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

class ImageWarping {
private:
	CImg<float> srcImg;
	CImg<float> result;
	int direction;
	vector<float> trans_matrix;
	vector<pair<int, int> > sort_corner;
public:
	CImg<float> run(string filename, vector<pair<int,int> > corner);
	vector<pair<int,int> > SortCorner(vector<pair<int,int> >& corner);
	vector<float> ComputeMatrix(vector<pair<int,int> > uv);
	pair<int, int> Transform(vector<float> matrix, pair<int,int> point);
};

#endif