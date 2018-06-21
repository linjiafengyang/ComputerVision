#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "CImg.h"
#include "Sift.h"

using namespace std;
using namespace cimg_library;

struct bestSift {
	float value;
	int x, y;
};

// you should tune these parameters
// fot Test-ImageData(1): scale=1, siftThresh=250
// for Test-ImageData(2): scale=0.25, siftThresh=250
const float scale = 0.25;
const float siftThresh = 250;
static float ratioThresh = 0.8;
static bool lastTwo = false;

class ImageStitching {
public:
	void run(string filepath, vector<string> filenames);
private:
	CImg<float> preprocess(CImg<float> image);
	CImg<float> convert2gray(CImg<float> image);
	CImg<float> imageStitching(CImg<float> image1, CImg<float> image2);
	CImg<float> imageBlending1(CImg<float> image1, CImg<float> image2);
	CImg<float> imageBlending2(float shiftX, float shiftY, CImg<float> image1, CImg<float> image2);
	float descriptorCount(SiftDescriptor descriptor1, SiftDescriptor descriptor2);
	float calculateShift(vector<bestSift> matches, vector<float> shift);
};
