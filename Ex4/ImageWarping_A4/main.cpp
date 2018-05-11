#include "ImageWarping.cpp"
#include <time.h>
#include <iostream>
int main() {
	CImg<float> src("Dataset/6.bmp");
	// 4.bmp需要将sigma设为5.5f
	// args: src sigma gradient_threshold vote_threshold peak_dis
	Hough hough(src, 10.5f, 30, 1000, 60);
	CImg<float> result = hough.imageWarping(src);
	result.display();
	result.save("result/6.bmp");
	return 0;
}