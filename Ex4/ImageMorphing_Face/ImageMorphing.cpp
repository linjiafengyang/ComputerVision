#include "ImageMorphing.h"

ImageMorphing::ImageMorphing(CImg<float> src, CImg<float> dst, vector<Point> src_points, 
				vector<Point> dst_points, vector<vector<int>> index, int frame_cnt) {
	this->src = src;
	this->dst = dst;
	this->src_points = src_points;
	this->dst_points = dst_points;
	this->index = index;
	this->frame_cnt = frame_cnt;
	setSrcTriangleList();
	setDstTriangleList();
	setMidTriangleList();
}
// src图的三角形
void ImageMorphing::setSrcTriangleList() {
	for (int i = 0; i < index.size(); i++) {
		Triangle src_triangle(src_points[index[i][0]], src_points[index[i][1]], src_points[index[i][2]]);
		src_triangle_list.push_back(src_triangle);
	}
}
// dst图的三角形
void ImageMorphing::setDstTriangleList() {
	for (int i = 0; i < index.size(); i++) {
		Triangle dst_triangle(dst_points[index[i][0]], dst_points[index[i][1]], dst_points[index[i][2]]);
		dst_triangle_list.push_back(dst_triangle);
	}
}
// morphing过程中每一帧的三角形
void ImageMorphing::setMidTriangleList() {
	for (int i = 0; i < frame_cnt; i++) {
		vector<Point> temp_mid_points;
		for (int j = 0; j < src_points.size(); j++) {
			float mid_x = float(src_points[j].x) + float(i+1)/(frame_cnt+1) * (dst_points[j].x - src_points[j].x);
			float mid_y = float(src_points[j].y) + float(i+1)/(frame_cnt+1) * (dst_points[j].y - src_points[j].y);
			Point mid_point(mid_x, mid_y);
			temp_mid_points.push_back(mid_point);
		}
		mid_points.push_back(temp_mid_points);
	}

	for (int i = 0; i < frame_cnt; i++) {
		vector<Triangle> temp_mid_triangles;
		for (int j = 0; j < index.size(); j++) {
			Triangle mid_triangle(mid_points[i][index[j][0]], mid_points[i][index[j][1]], mid_points[i][index[j][2]]);
			temp_mid_triangles.push_back(mid_triangle);
		}
		mid_triangle_list.push_back(temp_mid_triangles);
	}
}
// 重心法判断一个点是否在三角形内
bool ImageMorphing::isInTriangle(Point P, Triangle tri) {
	float x0 = tri.p3.x - tri.p1.x, y0 = tri.p3.y - tri.p1.y;
	float x1 = tri.p2.x - tri.p1.x, y1 = tri.p2.y - tri.p1.y;
	float x2 = P.x - tri.p1.x, y2 = P.y - tri.p1.y;

	float temp_00 = x0 * x0 + y0 * y0;
	float temp_01 = x0 * x1 + y0 * y1;
	float temp_02 = x0 * x2 + y0 * y2;
	float temp_11 = x1 * x1 + y1 * y1;
	float temp_12 = x1 * x2 + y1 * y2;
	
	float u = float(temp_11 * temp_02 - temp_01 * temp_12) / (float)(temp_00 * temp_11 - temp_01 * temp_01);
	float v = float(temp_00 * temp_12 - temp_01 * temp_02) / (float)(temp_00 * temp_11 - temp_01 * temp_01);
	if (u + v <= 1 && u >= 0 && v >= 0) return true;
	return false;
}
// 计算变换矩阵
CImg<float> ImageMorphing::computeTransformMatrix(Triangle before, Triangle after) {
	CImg<float> A(3, 3, 1, 1, 1);
	CImg<float> y1(1, 3, 1, 1, 0), y2(1, 3, 1, 1, 0);
	CImg<float> c1(1, 3, 1, 1, 0), c2(1, 3, 1, 1, 0);

	A(0, 0) = before.p1.x; A(1, 0) = before.p1.y;
	A(0, 1) = before.p2.x; A(1, 1) = before.p2.y;
	A(0, 2) = before.p3.x; A(1, 2) = before.p3.y;

	y1(0, 0) = after.p1.x; y2(0, 0) = after.p1.y;
	y1(0, 1) = after.p2.x; y2(0, 1) = after.p2.y;
	y1(0, 2) = after.p3.x; y2(0, 2) = after.p3.y;

	c1 = y1.solve(A);
	c2 = y2.solve(A);

	CImg<float> transform(3, 3, 1, 1, 0);
	for (int i = 0; i < 3; i++) {
		transform(i, 0) = c1(0, i);
		transform(i, 1) = c2(0, i);
	}
	transform(2, 2) = 1;
	return transform;
}
// 对每一帧进行morphing
CImgList<float> ImageMorphing::morphing() {
	int size = mid_triangle_list[0].size();

	result.push_back(src); // 放入第一帧
	for (int k = 0; k < frame_cnt; k++) {
		CImg<float> middle(dst._width, dst._height, 1, 3, 1);
		cimg_forXY(middle, i, j) {
			CImg<float> x(1, 3, 1, 1, 1), y1(1, 3, 1, 1, 1), y2(1, 3, 1, 1, 1);
			for (int m = 0; m < size; m++) {
				Point p(i, j);
				if (isInTriangle(p, mid_triangle_list[k][m])) {
					x(0, 0) = i;
					x(0, 1) = j;
					// middle image到src image的变换
					CImg<float> transform1 = computeTransformMatrix(mid_triangle_list[k][m], src_triangle_list[m]);
					y1 = transform1 * x;
					// middle image到dst image的变换
					CImg<float> transform2 = computeTransformMatrix(mid_triangle_list[k][m], dst_triangle_list[m]);
					y2 = transform2 * x;
					// src和dst组合得到middle
					float a = float(k+1)/(frame_cnt+1);
					middle(i, j, 0) = (1 - a) * src(y1(0, 0), y1(0, 1), 0) + a * dst(y2(0, 0), y2(0, 1), 0);
					middle(i, j, 1) = (1 - a) * src(y1(0, 0), y1(0, 1), 1) + a * dst(y2(0, 0), y2(0, 1), 1);
					middle(i, j, 2) = (1 - a) * src(y1(0, 0), y1(0, 1), 2) + a * dst(y2(0, 0), y2(0, 1), 2);
					break;
				}
			}
		}
		result.push_back(middle); // 放入中间morphing的每一帧
	}
	result.push_back(dst); // 放入最后一帧
	return result;
}
