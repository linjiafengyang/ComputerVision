#pragma once
#ifndef EDGE_DETECT_H
#define EDGE_DETECT_H

#include <iostream>
#include <string>
#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

class EdgeDetect {
public:
	EdgeDetect(string, string, string, int, int minR = 0, int maxR = 0); // 构造函数：输入图像，并输出图像边缘检测结果
	void toGrayScale(); // 灰度化处理
	vector<vector<float>> createFilter(int, int, float); // 产生高斯滤波器
	CImg<float> useFilter(CImg<float>&, vector<vector<float>>&); // 进行高斯滤波
	CImg<float> sobel(CImg<float>&, CImg<float>&); // 产生sobel算子
	CImg<float> nonMaxSupp(CImg<float>&, CImg<float>&); // 进行非最大化抑制
	CImg<float> threshold(CImg<float>&, int, int); // 双阈值处理

	void houghLinesTransform(CImg<float>&); // 霍夫直线变换
	void houghLinesDetect(); // 霍夫直线检测
	int getMaxHough(CImg<float>&, int&, int&, int&); // 计算霍夫空间直线交点
	void drawEdge(); // 描绘检测出的边缘
	void drawPoint(); // 描绘检测出的角点

	void houghCirclesTransform(CImg<float>&, int, int); // 霍夫圆变换
	void houghCirclesDetect(); // 霍夫圆检测
	void drawCircle(int); // 描绘检测出的圆形
private:
	CImg<float> image; // 原图像
	CImg<float> grayImage; // 灰度图像
	CImg<float> thresholdImage; // 经过阈值处理后的图像
	CImg<float> houghImage; // 霍夫空间图像
	CImg<float> outputImage; // 霍夫变换检测出来的图像
	vector<vector<float>> filter; // 滤波器
	vector<float> setSin; // sin集合
	vector<float> setCos; // cos集合

	int pointNumber; // 角点数
	vector<pair<int, int>> lines; // 检测到的直线集合
	vector<int> lineWeight; // 累加矩阵
	vector<int> sortLineWeight; // 从大到小排序的累加矩阵
	CImg<float> edge; // 边缘直线

	int circleNumber; // 检测圆个数
	int minRadius; // 圆周最小半径
	int maxRadius; // 圆周最大半径
	vector<pair<int, int>> circles; // 检测到的圆心集合
	vector<pair<int, int>> voteSet; // 投票集合
	vector<pair<int, int>> center; // 存放累加值最大的圆心对应坐标
	vector<int> circleWeight; // 累加矩阵
	vector<int> sortCircleWeight; // 从大到小排序的累加矩阵
};

#endif // !EDGE_DETECT_H
