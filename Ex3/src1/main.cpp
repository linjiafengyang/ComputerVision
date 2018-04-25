#include "Hough.cpp"

int main() {
	CImg<float> src("2.bmp");
	// args: src sigma gradient_threshold vote_threshold peak_dis
	Hough hough(src, 10.5f, 30, 1000, 60);
	CImg<float> result = hough.houghProcess(src);
	result.display();
	result.save("result/2.bmp");
	return 0;
}