#include "Process.h"
#include "Canny.h"

CImg<float> Process::preprocess(string filename){
	this->image.load(filename.c_str());
	CImg<float> temp = this->image;
	// resize
	this->scale = temp._width / 300;
	temp.resize(image._width / scale, image._height / scale);
	// 转灰度图
	CImg<float> grayImage = RGBtoGray(temp);

	return grayImage;
}
CImg<float> Process::RGBtoGray(const CImg<float>& input) {
	CImg<float> grayImage = CImg<float>(input._width, input._height, 1, 1);
	cimg_forXY(input, x, y) {
		float r = input._atXY(x,y,0,0);
		float g = input._atXY(x,y,0,1);
		float b = input._atXY(x,y,0,2);

		int newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
		grayImage._atXY(x,y) = newValue;
	}
	return grayImage;
}
int Process::getScale() {
	return scale;
}
