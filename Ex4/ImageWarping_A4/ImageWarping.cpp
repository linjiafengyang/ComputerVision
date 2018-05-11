#include "ImageWarping.h"

Hough::Hough(CImg<float> srcImg, double sigma, double gradient_threshold, double vot_threshold, double peak_dis) {
	this->hough_result = srcImg;
	this->sigma = sigma;
	this->gradient_threshold = gradient_threshold;
	this->vote_threshold = vot_threshold;
	this->peak_dis = peak_dis;
	this->x_min = 0;
	this->x_max = srcImg._width - 1; // 图像宽度
	this->y_min = 0;
	this->y_max = srcImg._height - 1; // 图像高度
}
CImg<float> Hough::imageWarping(CImg<float> srcImg) {
	this->grayImage = RGBtoGray(srcImg); // 转灰度图
	this->blurred_img = grayImage.get_blur(sigma); // 高斯滤波平滑
	this->houghspace = initHoughSpace(); // 初始化霍夫空间
	findPeaks(); // 找出霍夫空间中直线经过最多的点
	drawLines(); // 寻找并画出直线
	drawIntersections(); // 寻找并画出直线交点
	//hough_result.display();
	this->A4 = warping(srcImg); // image warping
	return A4;
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
	CImgList<float> gradient =  blurred_img.get_gradient("xy", 2);

	CImg<float> gradient_x = gradient[0]; // x方向上的梯度
	CImg<float> gradient_y = gradient[1]; // y方向上的梯度

	int maxp = (int)sqrt(grayImage._width*grayImage._width + grayImage._height*grayImage._height);
	CImg<float> hough_space(360, maxp, 1, 1, 0); // 初始化hough space
	
	cimg_forXY(grayImage, i, j) {
		double grad = sqrt(gradient_x(i, j)*gradient_x(i, j) + gradient_y(i, j)*gradient_y(i, j));
		if (grad > gradient_threshold) {
			grayImage(i, j) = grad;
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
			hough_result.draw_line(x0, y_min, x1, y_max, lines_color);
		}
		else {
			hough_result.draw_line(x_min, y0, x_max, y1, lines_color);
		}
	}

}
bool sortByDistance(Line a, Line b) {
	return a.distance < b.distance;
}
// 寻找并画出直线交点
void Hough::drawIntersections() {
	vector<Point> tempIntersections;
	int k = 0;
	for (int i = 0; i < lines.size(); i++) {
		for (int j = i + 1; j < lines.size(); j++) {
			double k0 = lines[i].k, k1 = lines[j].k;
			double b0 = lines[i].b, b1 = lines[j].b;

			double x = (b1 - b0) / (k0 - k1);
			double y = (k0*b1 - k1*b0) / (k0 - k1);

			if (x >= 0 && x < grayImage._width && y >= 0 && y < grayImage._height) {
				Point tempPoint(x, y, 0);
				tempIntersections.push_back(tempPoint);
				cout << "Intersection " << k++ << ": x = " << x << ", y = " << y << endl;
			}
		}
	}

	const double intersections_color[] = { 255, 0, 0 };
	vector<Line> sort_line;
	for (int i = 0; i < tempIntersections.size(); i++) {
		hough_result.draw_circle(tempIntersections[i].x, tempIntersections[i].y, 50, intersections_color);

		double temp_x = tempIntersections[i].x - tempIntersections[0].x;
		double temp_y = tempIntersections[i].y - tempIntersections[0].y;
		double dis = sqrt(temp_x*temp_x + temp_y*temp_y);
		Line temp(i, dis);
		sort_line.push_back(temp);
	}
	sort(sort_line.begin(), sort_line.end(), sortByDistance);
	intersections.clear();
	for (int i = 0; i < sort_line.size(); i++) {
		intersections.push_back(tempIntersections[sort_line[i].index]); // 四个角点
	}
}
// 计算变换矩阵
vector<CImg<float> > Hough::computeTransformMatrix(CImg<float> a4) {
	vector<Point> destRectPoints;
	Point tempPoint1(0, 0, 0);
	Point tempPoint2(a4._width - 1, 0, 0);
	Point tempPoint3(0, a4._height - 1, 0);
	Point tempPoint4(a4._width - 1, a4._height - 1, 0);
	destRectPoints.push_back(tempPoint1); destRectPoints.push_back(tempPoint2);
	destRectPoints.push_back(tempPoint3); destRectPoints.push_back(tempPoint4);

	CImg<float> y1(1, 3, 1, 1, 0), y2(1, 3, 1, 1, 0), y3(1, 3, 1, 1, 0), y4(1, 3, 1, 1, 0);
	CImg<float> c1(1, 3, 1, 1, 0), c2(1, 3, 1, 1, 0), c3(1, 3, 1, 1, 0), c4(1, 3, 1, 1, 0);
	CImg<float> A1(3, 3, 1, 1, 1), A2(3, 3, 1, 1, 1);

	for (int i = 0; i < 3; i++) {
		A1(0, i) = destRectPoints[i].x; A1(1, i) = destRectPoints[i].y;
		A2(0, i) = destRectPoints[3-i].x; A2(1, i) = destRectPoints[3-i].y;

		y1(0, i) = intersections[i].x; y2(0, i) = intersections[i].y;
		y3(0, i) = intersections[3-i].x; y4(0, i) = intersections[3-i].y;
	}
	c1 = y1.solve(A1); c2 = y2.solve(A1);
	c3 = y3.solve(A2); c4 = y4.solve(A2);

	CImg<float> temptransform1(3, 3, 1, 1, 0), temptransform2(3, 3, 1, 1, 0);
	for (int i = 0; i < 3; i++) {
		temptransform1(i, 0) = c1(0, i);
		temptransform1(i, 1) = c2(0, i);

		temptransform2(i, 0) = c3(0, i);
		temptransform2(i, 1) = c4(0, i);
	}
	temptransform1(0, 2) = 0; temptransform1(1, 2) = 0; temptransform1(2, 2) = 1;
	temptransform2(0, 2) = 0; temptransform2(1, 2) = 0; temptransform2(2, 2) = 1;
	vector<CImg<float> > temptransform;
	temptransform.push_back(temptransform1);
	temptransform.push_back(temptransform2);
	return temptransform;
}
// image warping
CImg<float> Hough::warping(CImg<float> srcImg) {
	// 当分辨率是150像素/英寸时，A4纸像素长宽分别是1754×1240
	// 当分辨率是300像素/英寸时，A4纸像素长宽分别是3508×2479
	CImg<float> a4(1240, 1754, 1, 3, 0);
	//CImg<float> a4(2479, 3508, 1, 3, 0);
	vector<CImg<float> > transform;
	transform = computeTransformMatrix(a4); // 计算变换矩阵

	CImg<float> y(1, 2, 1, 1, 0);
	CImg<float> c(1, 2, 1, 1, 0);
	CImg<float> A(2, 2, 1, 1, 1);
	A(0, 0) = 0;
	A(0, 1) = a4._width - 1;
	y(0, 0) = a4._height - 1;
	y(0, 1) = 0;
	c = y.solve(A);

	CImg<float> temp1(1, 3, 1, 1, 1), temp2(1, 3, 1, 1, 1);
	cimg_forXY(a4, i, j) {
		temp1(0, 0) = i;
		temp1(0, 1) = j;
		
		double inner_procuct = i * c(0, 0) - j + c(0, 1);
		temp2 = inner_procuct >= 0 ? transform[0] * temp1 : transform[1] * temp1;
		temp2(0, 0) = temp2(0, 0) < 0 ? 0 : (temp2(0, 0) > x_max ? x_max : temp2(0, 0));
		temp2(0, 1) = temp2(0, 1) < 0 ? 0 : (temp2(0, 1) > y_max ? y_max : temp2(0, 1));

		a4(i, j, 0) = srcImg(temp2(0, 0), temp2(0, 1), 0);
		a4(i, j, 1) = srcImg(temp2(0, 0), temp2(0, 1), 1);
		a4(i, j, 2) = srcImg(temp2(0, 0), temp2(0, 1), 2);
	}
	return a4;
}
