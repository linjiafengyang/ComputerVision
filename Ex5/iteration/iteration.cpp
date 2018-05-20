#include "iteration.h"

CImg<float> Iteration::run_iteration(const CImg<float>& image) {
	CImg<float> grayImage = RGBtoGray(image);
	threshold = iteration(grayImage);
	cout << threshold << endl;
	CImg<float> result = segment(image, threshold);
	return result;
}

int Iteration::iteration(const CImg<float>& image) {
	// 灰度直方图初始化为0
	for (int i = 0; i < 256; i++) {
		histogram[i] = 0;
	}
	pixelsNum = image.width() * image.height();
	// 计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数
	cimg_forXY(image, i, j) {
		++histogram[int(image(i, j, 0))];
	}

	threshold = 0;
	for (int i = 0; i < 256; i++) {
		threshold += i * histogram[i];
	}
	threshold /= pixelsNum; // 阈值初始化为总灰度和的平均值

	int threshold_new;
	while (1) {
		int t1 = 0, t2 = 0;
		int num1 = 0, num2 = 0;

		// 计算小于等于阈值threshold的灰度平均值t1
		for (int i = 0; i <= threshold; i++) {
			t1 += i * histogram[i];
			num1 += histogram[i];
		}
		if (num1 == 0) continue;
		t1 /= num1;

		// 计算大于阈值threshold的灰度平均值t2
		for (int i = threshold + 1; i < 256; i++) {
			t2 += i * histogram[i];
			num2 += histogram[i];
		}
		if (num2 == 0) continue;
		t2 /= num2;

		threshold_new = (t1 + t2) / 2;
		// 若两个阈值相等，则返回阈值threshold，否则更新阈值继续循环
		if (threshold == threshold_new) break;
		else threshold = threshold_new;
	}
	return threshold;
}

// 转灰度图
CImg<float> Iteration::RGBtoGray(const CImg<float>& image) {
	CImg<float> grayImage = CImg<float>(image._width, image._height, 1, 1, 0);
	cimg_forXY(grayImage, x, y) {
		grayImage(x, y, 0) = (int)round((double)image(x, y, 0, 0) * 0.299 + 
									(double)image(x, y, 0, 1) * 0.587 + 
									(double)image(x, y, 0, 2) * 0.114);
	}
	return grayImage;
}

CImg<float> Iteration::segment(CImg<float> image, int threshold) {
	cimg_forXY(image, i, j) {
		if (image(i, j) > threshold) {
			image(i, j, 0, 0) = 255;
			image(i, j, 0, 1) = 255;
			image(i, j, 0, 2) = 255;
		}
		else {
			image(i, j, 0, 0) = 0;
			image(i, j, 0, 1) = 0;
			image(i, j, 0, 2) = 0;
		}
	}
	return image;
}
