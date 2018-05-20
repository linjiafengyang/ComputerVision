#include "ostu.h"

CImg<float> OSTU::run_ostu(const CImg<float>& image) {
	CImg<float> grayImage = RGBtoGray(image);
	threshold = ostu(grayImage);
	cout << threshold << endl;
	CImg<float> result = segment(image, threshold);
	return result;
}
// ostu算法求阈值
int OSTU::ostu(const CImg<float>& image) {
	variance = 0; // 类间方差初始化为0
	// 灰度直方图初始化为0
	for (int i = 0; i < 256; i++) {
		histogram[i] = 0;
	}
	pixelsNum = image.width() * image.height(); // 像素点总数
	// 计算灰度直方图分布，Histogram数组下标是灰度值，保存内容是灰度值对应像素点数
	cimg_forXY(image, i, j) {
		++histogram[int(image(i, j, 0))];
	}

	for (int i = 0; i < 256; i++) {
		P1 = 0; P2 = 0; m1 = 0; m2 = 0;
		for (int j = 0; j <= i; j++) {
			P1 += histogram[j]; // 前景像素点总数
			m1 += j * histogram[j]; // 前景部分像素总灰度和
		}
		if (P1 == 0) continue;
		m1 /= P1; // 前景像素平均灰度
		P1 /= pixelsNum; // 前景像素点数所占比例

		for (int j = i + 1; j < 256; j++) {
			P2 += histogram[j]; // 背景像素点总数
			m2 += j * histogram[j]; // 背景部分像素总灰度和
		}
		if (P2 == 0) continue;
		m2 /= P2; // 背景像素平均灰度
		P2 /= pixelsNum; // 背景像素点数所占比例

		double temp_variance = P1 * P2 * (m1 - m2) * (m1 - m2); // 当前类间方差
		// 更新类间方差和阈值
		if (variance < temp_variance) {
			variance = temp_variance;
			threshold = i;
		}
	}
	return threshold;
}

// 转灰度图
CImg<float> OSTU::RGBtoGray(const CImg<float>& image) {
	CImg<float> grayImage = CImg<float>(image._width, image._height, 1, 1, 0);
	cimg_forXY(grayImage, x, y) {
		grayImage(x, y, 0) = (int)round((double)image(x, y, 0, 0) * 0.299 + 
									(double)image(x, y, 0, 1) * 0.587 + 
									(double)image(x, y, 0, 2) * 0.114);
	}
	return grayImage;
}

CImg<float> OSTU::segment(CImg<float> image, int threshold) {
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