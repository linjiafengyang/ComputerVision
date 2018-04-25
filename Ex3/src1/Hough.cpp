#include "Hough.h"

Hough::Hough(CImg<float> srcImg, double sigma, double gradient_threshold, double vot_threshold, double peak_dis) {
	this->result = srcImg;
	this->sigma = sigma;
	this->gradient_threshold = gradient_threshold;
	this->vote_threshold = vot_threshold;
	this->peak_dis = peak_dis;
	this->x_min = 0;
	this->x_max = srcImg._width - 1; // 图像宽度
	this->y_min = 0;
	this->y_max = srcImg._height - 1; // 图像高度
}
CImg<float> Hough::houghProcess(CImg<float> srcImg) {
	this->src = RGBtoGray(srcImg); // 转灰度图
	this->blurred_img = src.get_blur(sigma); // 高斯滤波平滑
	this->houghspace = initHoughSpace(); // 初始化霍夫空间
	findPeaks(); // 找出霍夫空间中直线经过最多的点
	drawLines(); // 寻找并画出直线
	drawIntersections(); // 寻找并画出直线交点
	return result;
}
// 转灰度图
CImg<float> Hough::RGBtoGray(const CImg<float>& srcImg) {
	CImg<float> grayImage = CImg<float>(srcImg._width, srcImg._height, 1, 1, 0);
	cimg_forXY(grayImage, x, y) {
		grayImage(x, y, 0) = (int)round((double)srcImg(x, y, 0, 0) * 0.299 + 
									(double)srcImg(x, y, 0, 1) * 0.587 + 
									(double)srcImg(x, y, 0, 2) * 0.114);
	}
	return grayImage;
}
// 初始化霍夫空间
CImg<float> Hough::initHoughSpace() {
	// sobel算子
	CImg<float> sobelx(3, 3, 1, 1, 0);
	CImg<float> sobely(3, 3, 1, 1, 0);
	sobelx(0, 0) = -1, sobely(0, 0) = 1;
	sobelx(0, 1) = 0, sobely(0, 1) = 2;
	sobelx(0, 2) = 1, sobely(0, 2) = 1;
	sobelx(1, 0) = -2, sobely(1, 0) = 0;
	sobelx(1, 1) = 0, sobely(1, 1) = 0;
	sobelx(1, 2) = 2, sobely(1, 2) = 0;
	sobelx(2, 0) = -1, sobely(2, 0) = -1;
	sobelx(2, 1) = 0, sobely(2, 1) = -2;
	sobelx(2, 2) = 1, sobely(2, 2) = -1;

	CImg<float> gradient_x = blurred_img;
	gradient_x = gradient_x.get_convolve(sobelx); // 计算x方向上的梯度
	CImg<float> gradient_y = blurred_img;
	gradient_y = gradient_y.get_convolve(sobely); // 计算y方向上的梯度

	int maxp = (int)sqrt(src._width*src._width + src._height*src._height);
	CImg<float> hough_space(360, maxp, 1, 1, 0); // 初始化hough space
	
	cimg_forXY(src, i, j) {
		double grad = sqrt(gradient_x(i, j)*gradient_x(i, j) + gradient_y(i, j)*gradient_y(i, j));
		if (grad > gradient_threshold) {
			src(i, j) = grad;
			cimg_forX(hough_space, alpha) {
				double theta = ((double)alpha*cimg::PI) / 180;
				int p = (int)(i*cos(theta) + j*sin(theta));
				if (p >= 0 && p < maxp) {
					hough_space(alpha, p)++; // 累加矩阵
				}
			}
		}
	}
	return hough_space;
}
// 投票算法找出霍夫空间中直线经过最多的点
void Hough::findPeaks() {
	peaks.clear();
	cimg_forXY(houghspace, theta, p) {
		if (houghspace(theta, p) > vote_threshold) {
			bool flag = true;
			double alpha = (double)theta*cimg::PI / 180;
			// y的范围
			const int y0 = ((double)p / (sin(alpha))) - double(x_min)*(1 / tan(alpha));
			const int y1 = ((double)p / (sin(alpha))) - double(x_max)*(1 / tan(alpha));
			// x的范围
			const int x0 = ((double)p / (cos(alpha))) - double(y_min)*(tan(alpha));
			const int x1 = ((double)p / (cos(alpha))) - double(y_max)*(tan(alpha));

			if (x0 >= x_min && x0 <= x_max || x1 >= x_min && x1 <= x_max ||
				y0 >= y_min && y0 <= y_max || y1 >= y_min && y1 <= y_max) {
				for (int i = 0; i < peaks.size(); i++) {
					if (sqrt((peaks[i].x - theta)*(peaks[i].x - theta) 
						+ (peaks[i].y - p)*(peaks[i].y - p)) < peak_dis) {
						flag = false;
						if (peaks[i].cnt < houghspace(theta, p)) {
							Point temp(theta, p, houghspace(theta, p));
							peaks[i] = temp;
						}
					}
				}
				if (flag) {
					Point temp(theta, p, houghspace(theta, p));
					peaks.push_back(temp);
				}
			}
		}
	}
}
// 寻找并画出直线
void Hough::drawLines() {
	lines.clear();
	for (int i = 0; i < peaks.size(); i++) {
		double theta = double(peaks[i].x)*cimg::PI / 180;
		double k = -cos(theta) / sin(theta); // 直线斜率
		double b = double(peaks[i].y) / sin(theta);
		Line templine(k, b);
		lines.push_back(templine);
		cout << "Line " << i << ": y = " << k << "x + " << b << endl;
	}

	const double lines_color[] = { 255, 0, 0 };
	for (int i = 0; i < lines.size(); i++) {
		const int x0 = (double)(y_min - lines[i].b) / lines[i].k;
		const int x1 = (double)(y_max - lines[i].b) / lines[i].k;
		const int y0 = x_min*lines[i].k + lines[i].b;
		const int y1 = x_max*lines[i].k + lines[i].b;

		if (abs(lines[i].k) > 1) {
			result.draw_line(x0, y_min, x1, y_max, lines_color);
		}
		else {
			result.draw_line(x_min, y0, x_max, y1, lines_color);
		}
	}

}
// 寻找并画出直线交点
void Hough::drawIntersections() {
	intersections.clear();
	int k = 0;
	for (int i = 0; i < lines.size(); i++) {
		for (int j = i + 1; j < lines.size(); j++) {
			double k0 = lines[i].k;
			double k1 = lines[j].k;
			double b0 = lines[i].b;
			double b1 = lines[j].b;

			double x = (b1 - b0) / (k0 - k1);
			double y = (k0*b1 - k1*b0) / (k0 - k1);

			if (x >= 0 && x < src._width && y >= 0 && y < src._height) {
				Point tempPoint(x, y, 0);
				intersections.push_back(tempPoint);
				cout << "Intersection " << k++ << ": x = " << x << ", y = " << y << endl;
			}
		}
	}

	const double intersections_color[] = { 255, 0, 0 };
	for (int i = 0; i < intersections.size(); i++) {
		result.draw_circle(intersections[i].x, intersections[i].y, 50, intersections_color);
	}
}