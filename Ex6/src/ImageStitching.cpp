#include "ImageStitching.h"

void ImageStitching::run(string filepath, vector<string> filenames) {
	// read image
	vector<CImg<float> > image_array;
	sort(filenames.begin(), filenames.end());
	for(int i = 0; i < filenames.size(); ++i) {
		string image_filepath = filepath + filenames[i];
		CImg<float> image;
		image.load(image_filepath.c_str());
		// preprocess image
		image = preprocess(image);
		image_array.push_back(image);
	}

	// image stitching
	int cnt = 0;
	while(image_array.size() != 1) {
		ratioThresh = ratioThresh - 0.06*cnt;
		cnt++;
		vector<CImg<float> > stitchedImage;
		for(int i = 0; i < image_array.size()/2; ++i) {
			CImg<float> image = imageStitching(image_array[2*i], image_array[2*i+1]);
			if(image_array.size()%2 == 1 && image_array.size()-1 == 2*i+2)
				image = imageStitching(image, image_array[2*i+2]);
			//image.display();
			stitchedImage.push_back(image);
		}
		image_array = stitchedImage;
		if(image_array.size() == 2)
			lastTwo = true;
	}
	CImg<float> finalResult = image_array[0];
	finalResult.display();
	finalResult.save("../result/test1.jpg");
}

// stitch two images
CImg<float> ImageStitching::imageStitching(CImg<float> image1, CImg<float> image2) {
	// convert image1 to gray image and compute sift
	CImg<float> grayImage1 = convert2gray(image1);
	vector<SiftDescriptor> descriptor1 = Sift::compute_sift(grayImage1);
	// convert image2 to gray image and compute sift
	CImg<float> grayImage2 = convert2gray(image2);
	vector<SiftDescriptor> descriptor2 = Sift::compute_sift(grayImage2);

	// put the two images into one big image
	CImg<float> siftResult = imageBlending1(image1, image2);

	vector<float> shiftx, shifty;
	bestSift bestSift1, bestSift2;
	vector<bestSift> matches;
	const unsigned char red[] = { 255,0,0 };

	for(int x = 0; x < descriptor1.size(); x++) {
		float distance1 = 10000;
		float distance2 = 10000;
		float temp = 0;
		for(int y = 0; y < descriptor2.size(); y++) {
			temp = descriptorCount(descriptor1[x], descriptor2[y]);
			// find the lowest x and y distances for sift differences
			if(temp < distance1) {
				distance1 = temp;
				bestSift1.value = distance1;
				bestSift1.x = x;
				bestSift1.y = y;
			}
			else if(temp < distance2) {
				distance2 = temp;
				bestSift2.value = distance2;
				bestSift2.x = x;
				bestSift2.y = y;
			}
		}

		float ratio = bestSift1.value / bestSift2.value;

		if ((ratio < ratioThresh) && (bestSift1.value < siftThresh)) {
			if(lastTwo == true) {
				if(descriptor1[bestSift1.x].col < descriptor2[bestSift1.y].col)
					continue;
			}
			matches.push_back(bestSift1);
			
			for(int j = 0; j < 5; j++) {
				for(int k = 0; k < 5; k++) {
					if(j == 2 || k == 2) {
						for(int p = 0; p < 3; p++) {
							siftResult(descriptor1[bestSift1.x].col+k, 
								descriptor1[bestSift1.x].row+j, 0, p) = 0;
							siftResult(descriptor2[bestSift1.y].col+k+image1._width, 
								descriptor2[bestSift1.y].row+j, 0, p) = 0;
						}
					}
				}
			}
			// draw matching sift features between two images
			siftResult.draw_line(descriptor1[bestSift1.x].col, descriptor1[bestSift1.x].row,
					descriptor2[bestSift1.y].col+image1._width, descriptor2[bestSift1.y].row, red);

			float tempcol = descriptor1[bestSift1.x].col - descriptor2[bestSift1.y].col;
			float temprow = descriptor1[bestSift1.x].row - descriptor2[bestSift1.y].row;
			
			shiftx.push_back(tempcol);
			shifty.push_back(temprow);
		}
	}
	siftResult.display();
	//cout << "Th Number of matching sift features: " << matches.size() << endl;

	float shiftX = calculateShift(matches, shiftx);
	float shiftY = calculateShift(matches, shifty);
	// blend two images
	CImg<float> stitchResult = imageBlending2(shiftX, shiftY, image1, image2);
	return stitchResult;
}
// resize image
CImg<float> ImageStitching::preprocess(CImg<float> image) {
	CImg<float> temp = image;
	return temp.resize(temp._width*scale, temp._height*scale);
}
// convert rgbImage to grayImage
CImg<float> ImageStitching::convert2gray(CImg<float> image) {
	CImg<float> grayImage = CImg<float>(image._width,image._height,1,1);
	cimg_forXY(image, x, y) {
		grayImage._atXY(x,y) = 
			(image._atXY(x,y,0,0) * 0.299 + image._atXY(x,y,0,1) * 0.587 + image._atXY(x,y,0,2) * 0.114);	
	}
	return grayImage;
}
// put the two images into one big image
CImg<float> ImageStitching::imageBlending1(CImg<float> image1, CImg<float> image2) {
	int height = image1._height >= image2._height ? image1._height : image2._height;
	CImg<float> siftResult(image1._width+image2._width, height, 1, 3, 0);
	for (int y = 0; y < image1._height; y++) {
		for (int x = 0; x < image1._width; x++) {
			siftResult(x,y,0,0) = image1(x,y,0,0);
			siftResult(x,y,0,1) = image1(x,y,0,1);
			siftResult(x,y,0,2) = image1(x,y,0,2);
		}
	}
	for (int y = 0; y < image2._height; y++) {
		for (int x = image1._width; x < siftResult._width; x++) {
			siftResult(x,y,0,0) = image2(x-image1._width,y,0,0);
			siftResult(x,y,0,1) = image2(x-image1._width,y,0,1);
			siftResult(x,y,0,2) = image2(x-image1._width,y,0,2);
		}
	}
	return siftResult;
}
// blend two images
CImg<float> ImageStitching::imageBlending2(float shiftX, float shiftY, CImg<float> image1, CImg<float> image2) {
	float newWidth = abs(shiftX) + image1._width;
	float newHeight = abs(shiftY) + image1._height;
	CImg<float> blendResult(newWidth,newHeight,1,3,0);

	// blend image1
	int startX1 = shiftX < 0 ? abs(shiftX) : 0;
	int startY1 = shiftY < 0 ? abs(shiftY) : 0;
	int y1 = 0;
	for (int y = startY1; y < image1._height+startY1; y++) {
		int x1 = 0;
		for (int x = startX1; x < image1._width+startX1; x++) {
			blendResult(x,y,0,0) = image1(x1,y1,0,0);
			blendResult(x,y,0,1) = image1(x1,y1,0,1);
			blendResult(x,y,0,2) = image1(x1,y1,0,2);
			x1++;
		}
		y1++;
	}

	// blend image2
	int startX2 = shiftX > 0 ? shiftX : 0;
	int startY2 = shiftY > 0 ? shiftY : 0;
	y1 = 0;
	for (int y = startY2; y < image2._height+startY2; y++) {
		int x1 = 0;
		for (int x = startX2; x < image2._width+startX2; x++) {
			blendResult(x,y,0,0) = image2(x1,y1,0,0);
			blendResult(x,y,0,1) = image2(x1,y1,0,1);
			blendResult(x,y,0,2) = image2(x1,y1,0,2);
			x1++;
		}
		y1++;
	}
	return blendResult;
}
// calculate x or y best shift
float ImageStitching::calculateShift(vector<bestSift> matches, vector<float> shift) {
	float max, cur, curCount;
	float maxCount = 0;
	for (int i = 0; i < matches.size(); i++) {
		curCount = 0;
		cur = shift[i];
		for(int j = 0; j < shift.size(); j++) {
			if(abs(cur-shift[j]) <= 3) 
				curCount++;
		}
		if(curCount > maxCount) {
			maxCount = curCount;
			max = cur;
		}
	}
	return max;
}
float ImageStitching::descriptorCount(SiftDescriptor descriptor1, SiftDescriptor descriptor2) {
	float total = 0;
	for(int i = 0; i < 128; i++) 
		total += pow((descriptor1.descriptor[i] - descriptor2.descriptor[i]), 2.0);
	return sqrt(total);
}
