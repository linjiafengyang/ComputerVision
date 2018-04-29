#include "EdgeDetect.h"
#include "CANNY.h"
#include <cmath>
#include <algorithm>
#include <functional>

#define PI 3.141592653
#define gFilterX 5
#define gFilterY 5
#define sigma 1
#define thresholdLow 120
#define thresholdHigh 140
#define thetaSize 360
#define windowSize 60

EdgeDetect::EdgeDetect(string input, string output, string mode, int number, int minR, int maxR) {
	for (int i = 0; i < thetaSize; i++) {
		setSin.push_back(sin(2 * PI * i / thetaSize));
		setCos.push_back(cos(2 * PI * i / thetaSize));
	}

	if (mode == "line") {
		image.load(input.c_str());

		int width = image._width, height = image._height;
		image.resize(600, 800);
		if (width == 0 || height == 0) {
			cout << "Cannot open or find the image" << endl;
		}
		image.display("Origin Image"); // ÏÔÊ¾Ô­Í¼Ïñ
		outputImage = image;

		toGrayScale(); // ½øÐÐ»Ò¶È»¯´¦Àí
		filter = createFilter(gFilterX, gFilterY, sigma); // ²úÉú¸ßË¹ÂË²¨Æ÷
		CImg<float> gFiltered = useFilter(grayImage, filter); // ½øÐÐ¸ßË¹ÂË²¨

		/*Canny¼ì²â*/
		CImg<float> angles;
		CImg<float> sFiltered = sobel(gFiltered, angles); // ²úÉúsobelËã×Ó²¢¼ÆËãÌÝ¶È·ùÖµºÍ½Ç¶ÈÍ¼Ïñ
		CImg<float> nms = nonMaxSupp(sFiltered, angles); // ·Ç×î´ó»¯ÒÖÖÆ´¦Àí
		thresholdImage = threshold(nms, thresholdLow, thresholdHigh); // Ë«ãÐÖµ´¦Àí
		thresholdImage.display("Threshold Image"); // ÏÔÊ¾canny¼ì²â³öµÄ±ßÔµ

		for (int i = 0; i < thetaSize; i++) {
			setSin.push_back(sin(2 * PI * i / thetaSize));
			setCos.push_back(cos(2 * PI * i / thetaSize));
		}

		pointNumber = number;
		houghLinesTransform(thresholdImage); // »ô·òÖ±Ïß±ä»»
		houghLinesDetect(); // »ô·òÖ±Ïß¼ì²â

		drawEdge(); // Ãè»æ»ô·ò±ä»»¼ì²â³öµÄ±ßÔµ
		drawPoint(); // Ãè»æ»ô·ò±ä»»¼ì²â³öµÄ½Çµã
		outputImage.resize(width, height);
		outputImage.save(output.c_str());
	}
	else if (mode == "circle") {
		image.load(input.c_str());

		int width = image._width, height = image._height;
		if (width == 0 || height == 0) {
			cout << "Cannot open or find the image" << endl;
		}
		image.display("Origin Image"); // ÏÔÊ¾Ô­Í¼Ïñ
		outputImage = image;

		CImg<unsigned char> crImage(image);
		CANNY myCanny;
		crImage = myCanny.toGrayScale(crImage);
		unsigned char* grey = crImage._data;
		myCanny.canny(grey, crImage.width(), crImage.height(), 2.5f, 7.5f, 4.5f, 16);
		CImg<double> edge(myCanny.result, image.width(), image.height());
		thresholdImage = edge;
		thresholdImage.display("Threshold Image"); // ÏÔÊ¾canny¼ì²â³öµÄ±ßÔµ

		circleNumber = number;
		minRadius = minR;
		maxRadius = maxR;

		houghCirclesTransform(thresholdImage, minRadius, maxRadius); // »ô·òÔ²±ä»»
		outputImage.save(output.c_str());
	}
}

void EdgeDetect::toGrayScale() {
	grayImage = CImg<float>(image._width, image._height, 1, 1); //ÐÂ½¨Ò»¸ö»Ò¶ÈÍ¼Ïñ 
	//²ÊÉ«Í¼Ïñ×ªÎª»Ò¶ÈÍ¼ÏñµÄ¹«Ê½£ºR * 0.2989 + G * 0.5870 + B * 0.1140
	cimg_forXY(image, x, y) {
		grayImage(x, y) = image(x, y, 0) * 0.2989 + image(x, y, 1) * 0.5870 + image(x, y, 2) * 0.1140;
	}
}

/*¸ßË¹ÂË²¨Æ÷*/
vector<vector<float>> EdgeDetect::createFilter(int row, int col, float tempSigma) {
	float sum = 0, temp = 2.0 * tempSigma * tempSigma;
	/*³õÊ¼»¯*/
	for (int i = 0; i < row; i++) {
		vector<float> v(col, 0);
		filter.push_back(v);
	}

	for (int i = -row / 2; i <= row / 2; i++) {
		for (int j = -col / 2; j <= col / 2; j++) {
			filter[i + row / 2][j + col / 2] = exp(-(i * i + j * j) / temp) / sqrt(PI * temp); // ¸ßË¹º¯Êý
			sum += filter[i + row / 2][j + col / 2];
		}
	}
	// ¹éÒ»»¯
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			filter[i][j] /= sum;
		}
	}
	return filter;
}

/*½øÐÐ¸ßË¹ÂË²¨*/
CImg<float> EdgeDetect::useFilter(CImg<float>& img, vector<vector<float>>& filt) {
	int size = filt.size() / 2;
	CImg<float> filtered(img._width - 2 * size, img._height - 2 * size, 1, 1);
	for (int i = size; i < img._width - size; i++) {
		for (int j = size; j < img._height - size; j++) {
			float sum = 0;
			for (int x = 0; x < filt.size(); x++) {
				for (int y = 0; y < filt.size(); y++) {
					sum += filt[x][y] * (float)(img(i + x - size, j + y - size)); // ¸ßË¹ÂË²¨
				}
			}
			filtered(i - size, j - size) = sum;
		}
	}
	return filtered;
}

/*ÀûÓÃsobelËã×Ó¼ÆËãÌÝ¶È·ùÖµºÍ½Ç¶ÈÍ¼Ïñ*/
CImg<float> EdgeDetect::sobel(CImg<float>& gFiltered, CImg<float>& angles) {
	/*sobelËã×Ó*/
	vector<vector<float>> xFilter(3, vector<float>(3, 0)), yFilter(3, vector<float>(3, 0));
	xFilter[0][0] = xFilter[2][0] = yFilter[0][0] = yFilter[0][2] = -1;
	xFilter[0][2] = xFilter[2][2] = yFilter[2][0] = yFilter[2][2] = 1;
	xFilter[1][0] = yFilter[0][1] = -2;
	xFilter[1][2] = yFilter[2][1] = 2;

	int size = xFilter.size() / 2;
	CImg<float> filteredImage(gFiltered._width - 2 * size, gFiltered._height - 2 * size, 1, 1);
	angles = filteredImage;

	for (int i = size; i < gFiltered._width - size; i++) {
		for (int j = size; j < gFiltered._height - size; j++) {
			/*¼ÆËãÌÝ¶È·ù¶Ègx,gy*/
			float sumX = 0, sumY = 0;
			for (int x = 0; x < xFilter.size(); x++) {
				for (int y = 0; y < yFilter.size(); y++) {
					sumX += xFilter[y][x] * (float)(gFiltered(i + x - size, j + y - size));
					sumY += yFilter[y][x] * (float)(gFiltered(i + x - size, j + y - size));
				}
			}
			if (sqrt(sumX * sumX + sumY * sumY) > 255) {
				filteredImage(i - size, j - size) = 255;
			}
			else {
				filteredImage(i - size, j - size) = sqrt(sumX * sumX + sumY * sumY);
			}

			/*¼ÆËãÌÝ¶È·½Ïò*/
			if (sumX == 0) {
				angles(i - size, j - size) = 90;
			}
			else {
				angles(i - size, j - size) = atan(sumY / sumX);
			}
		}
	}
	return filteredImage;
}

/*¶ÔÌÝ¶È·ùÖµÍ¼ÏñÓ¦ÓÃ·Ç×î´ó»¯ÒÖÖÆ*/
CImg<float> EdgeDetect::nonMaxSupp(CImg<float>& sFiltered, CImg<float>& angles) {
	CImg<float> nms(sFiltered._width - 2, sFiltered._height - 2, 1, 1);
	for (int i = 1; i < sFiltered._width - 1; i++) {
		for (int j = 1; j < sFiltered._height - 1; j++) {
			float angle = angles(i, j);
			nms(i - 1, j - 1) = sFiltered(i, j);

			/*Ë®Æ½±ßÔµ*/
			if ((angle > -22.5 && angle <= 22.5) || (angle > 157.5 && angle <= -157.5)) {
				if (sFiltered(i, j) < sFiltered(i, j + 1) || sFiltered(i, j) < sFiltered(i, j - 1)) {
					nms(i - 1, j - 1) = 0;
				}
			}
			/*+45¶È±ßÔµ*/
			if ((angle > -67.5 && angle <= -22.5) || (angle > 112.5 && angle <= 157.5)) {
				if (sFiltered(i, j) < sFiltered(i - 1, j + 1) || sFiltered(i, j) < sFiltered(i + 1, j - 1)) {
					nms(i - 1, j - 1) = 0;
				}
			}
			/*´¹Ö±±ßÔµ*/
			if ((angle > -112.5 && angle <= -67.5) || (angle > 67.5 && angle <= 112.5)) {
				if (sFiltered(i, j) < sFiltered(i + 1, j) || sFiltered(i, j) < sFiltered(i - 1, j)) {
					nms(i - 1, j - 1) = 0;
				}
			}
			/*-45¶È±ßÔµ*/
			if ((angle > -157.5 && angle <= -112.5) || (angle > 22.5 && angle <= 67.5)) {
				if (sFiltered(i, j) < sFiltered(i + 1, j + 1) || sFiltered(i, j) < sFiltered(i - 1, j - 1)) {
					nms(i - 1, j - 1) = 0;
				}
			}
		}
	}
	return nms;
}

/*ÓÃË«ãÐÖµ´¦ÀíºÍÁ¬½Ó·ÖÎöÀ´¼ì²â²¢Á¬½Ó±ßÔµ*/
CImg<float> EdgeDetect::threshold(CImg<float>& img, int low, int high) {
	low = (low > 255) ? 255 : low;
	high = (high > 255) ? 255 : high;

	CImg<float> edgeMatch(img._width, img._height, 1, 1);
	for (int i = 0; i < img._width; i++) {
		for (int j = 0; j < img._height; j++) {
			edgeMatch(i, j) = img(i, j);
			if (edgeMatch(i, j) > high) {
				edgeMatch(i, j) = 255; // Èç¹û¸ßÓÚ¸ßãÐÖµ£¬¸³ÖµÎª255
			}
			else if (edgeMatch(i, j) < low) {
				edgeMatch(i, j) = 0; // Èç¹ûµÍÓÚµÍãÐÖµ£¬¸³ÖµÎª0
			}
			else {
				bool ifHigh = false, ifBetween = false;
				for (int x = i - 1; x < i + 2; x++) {
					for (int y = j - 1; y < j + 2; y++) {
						if (x > 0 && x <= edgeMatch._height && y > 0 && y <= edgeMatch._width) {
							if (edgeMatch(x, y) > high) {
								edgeMatch(i, j) = 255;
								ifHigh = true;
								break;
							}
							else if (edgeMatch(x, y) <= high && edgeMatch(x, y) >= low) {
								ifBetween = true;
							}
						}
					}
					if (ifHigh) {
						break;
					}
				}
				if (!ifHigh && ifBetween) {
					for (int x = i - 2; x < i + 3; x++) {
						for (int y = j - 1; y < j + 3; y++) {
							if (x > 0 && x <= edgeMatch._height && y > 0 && y <= edgeMatch._width) {
								if (edgeMatch(x, y) > high) {
									edgeMatch(i, j) = 255;
									ifHigh = true;
									break;
								}
							}
						}
						if (ifHigh) {
							break;
						}
					}
				}
				if (!ifHigh) {
					edgeMatch(i, j) = 0;
				}
			}
		}
	}
	return edgeMatch;
}

/*»ô·òÖ±Ïß±ä»»*/
void EdgeDetect::houghLinesTransform(CImg<float>& img) {
	int width = img._width, height = img._height, maxLength, row, col;
	maxLength = sqrt(pow(width / 2, 2) + pow(height / 2, 2)); // ½øÐÐ»ô·ò¿Õ¼ä¼«×ø±ê±ä»»
	row = thetaSize;
	col = maxLength;

	houghImage = CImg<float>(col, row);
	houghImage.fill(0);

	cimg_forXY(img, x, y) {
		int value = img(x, y), p = 0;
		if (value != 0) {
			int x0 = x - width / 2, y0 = height / 2 - y;
			for (int i = 0; i < thetaSize; i++) {
				/*½øÐÐvotingÍ¶Æ±*/
				p = x0 * setCos[i] + y0 * setSin[i];
				if (p >= 0 && p < maxLength) {
					houghImage(p, i)++;
				}
			}
		}
	}
}

/*»ô·òÖ±Ïß¼ì²â*/
void EdgeDetect::houghLinesDetect() {
	int width = houghImage._width, height = houghImage._height, size = windowSize, max;
	for (int i = 0; i < height; i += size / 2) {
		for (int j = 0; j < width; j += size / 2) {
			max = getMaxHough(houghImage, size, i, j);
			for (int y = i; y < i + size; ++y) {
				for (int x = j; x < j + size; ++x) {
					if (houghImage._atXY(x, y) < max) {
						houghImage._atXY(x, y) = 0; // °Ñ²»ÊÇ±ßÔµµãµÄµãÈ¥µô
					}	
				}
			}
		}
	}
	/*½«»ô·òÍ¼ÏñÖÐËùÓÐ²»Îª0µÄµã¶ÔÓ¦Ö±ÏßµÄÐ±ÂÊºÍ½Ø¾à´æÈëÊý×é*/
	cimg_forXY(houghImage, x, y) {
		if (houghImage(x, y) != 0) {
			lines.push_back(make_pair(y, x));
			lineWeight.push_back(houghImage(x, y));
		}
	}
}

/*¼ÆËã»ô·ò¿Õ¼äÖ±Ïß½»µã*/
int EdgeDetect::getMaxHough(CImg<float>& img, int& size, int& y, int& x) {
	int width = (x + size > img._width) ? img._width : x + size;
	int height = (y + size > img._height) ? img._height : y + size;
	int max = 0;
	for (int j = x; j < width; j++) {
		for (int i = y; i < height; i++) {
			max = (img(j, i) > max) ? img(j, i) : max;
		}
	}
	return max;
}

/*Ãè»æËù¼ì²â³öµÄ±ßÔµ*/
void EdgeDetect::drawEdge() {
	int width = image._width, height = image._height, maxLength;
	maxLength = sqrt(pow(width / 2, 2) + pow(height / 2, 2));

	edge = CImg<float>(width, height, 1, 1, 0);
	sortLineWeight = lineWeight;
	sort(sortLineWeight.begin(), sortLineWeight.end(), greater<int>()); // ½«ÀÛ¼Ó¾ØÕó´Ó´óµ½Ð¡½øÐÐÅÅÐò

	vector<pair<int, int>> result; // ´æ·ÅÀÛ¼ÓÖµ×î´óµÄ±ßÔµÖ±Ïß¶ÔÓ¦Ð±ÂÊºÍ½Ø¾à
	for (int i = 0; i < pointNumber; i++) {
		int weight = sortLineWeight[i], index;
		vector<int>::iterator iter = find(lineWeight.begin(), lineWeight.end(), weight);
		index = iter - lineWeight.begin();
		result.push_back(lines[index]);
	}
	for (int i = 0; i < result.size(); i++) {
		int theta = result[i].first, p = result[i].second;
		/*¸ù¾ÝthetaºÍpÇó³öÐ±ÂÊºÍ½Ø¾à*/
		cimg_forXY(edge, x, y) {
			int x0 = x - width / 2, y0 = height / 2 - y;
			if (p == (int)(x0 * setCos[theta] + y0 * setSin[theta])) {
				edge(x, y) += 255.0 / 2;
				outputImage(x, y, 0, 2) = 255;
			}
		}
	}
}

/*Ãè»æËù¼ì²â³öµÄ½Çµã*/
void EdgeDetect::drawPoint() {
	unsigned char red[3] = { 255, 0, 0 };
	for (int y = 0; y < outputImage._height - 1; y++) {
		for (int x = 0; x < outputImage._width - 1; x++) {
			int arr[4];
			arr[0] = edge(x, y);
			arr[1] = edge(x + 1, y);
			arr[2] = edge(x, y + 1);
			arr[3] = edge(x + 1, y + 1);
			if (arr[0] + arr[1] + arr[2] + arr[3] >= 255.0 * 3 / 2) {
				outputImage.draw_circle(x, y, 3, red);
			}
		}
	}
	outputImage.display("Point Detect");
}

/*»ô·òÔ²±ä»»*/
void EdgeDetect::houghCirclesTransform(CImg<float>& img, int minR, int maxR) {
	int width = img._width, height = img._height, max = 0;

	for (int r = minR; r < maxR; r += 5) {
		max = 0;
		houghImage = CImg<float>(width, height);
		houghImage.fill(0);
		cimg_forXY(img, x, y) {
			int value = img(x, y);
			if (value != 0) {
				for (int i = 0; i < thetaSize; i++) {
					int x0 = x - r * setCos[i];
					int y0 = y - r * setSin[i];
					/*½øÐÐvotingÍ¶Æ±*/
					if (x0 > 0 && x0 < width && y0 > 0 && y0 < height) {
						houghImage(x0, y0)++;
					}
				}
			}
		}

		/*Ã¿´Î±éÀúÍêrºó£¬ÕÒµ½houghÀïÃæµÄ×î´óÍ¶Æ±Êý£¬Õâ¸öÍ¶Æ±Êý±íÊ¾µ±Ç°rµÄÎÇºÏ³Ì¶È£¬È»ºóÓÃÍ¶Æ±Êý×î´óµÄr×÷Îª×îºÃµÄr*/
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				if (houghImage(x, y) > max) {
					max = houghImage(x, y);
				}
			}
		}
		voteSet.push_back(make_pair(max, r));
	}
	sort(voteSet.begin(), voteSet.end(), [](const pair<int, int>& x, const pair<int, int>& y) -> int {
		return x.first > y.first;
	});

	for (int i = 0; i < circleNumber; i++) {
		houghImage = CImg<float>(width, height);
		houghImage.fill(0);
		cimg_forXY(img, x, y) {
			int value = img(x, y);
			if (value != 0) {
				for (int j = 0; j < thetaSize; j++) {
					int x0 = x - voteSet[i].second * setCos[j];
					int y0 = y - voteSet[i].second * setSin[j];
					/*½øÐÐvotingÍ¶Æ±*/
					if (x0 > 0 && x0 < width && y0 > 0 && y0 < height) {
						houghImage(x0, y0)++;
					}
				}
			}
		}
		cout << "The radius is " << voteSet[i].second << endl;
		houghCirclesDetect();
		drawCircle(voteSet[i].second);
	}
	outputImage.display("Circle Detect");
}

void EdgeDetect::houghCirclesDetect() {
	/*½«»ô·òÍ¼ÏñÖÐËùÓÐ²»Îª0µÄµã¶ÔÓ¦Ô²ÐÄµÄ×ø±ê´æÈëÊý×é*/
	cimg_forXY(houghImage, x, y) {
		if (houghImage(x, y) != 0) {
			circles.push_back(make_pair(x, y));
			circleWeight.push_back(houghImage(x, y));
		}
	}
}

void EdgeDetect::drawCircle(int r) {
	int width = image._width, height = image._height, count = 0;
	unsigned char red[3] = { 255, 0, 0 };

	sortCircleWeight = circleWeight;
	sort(sortCircleWeight.begin(), sortCircleWeight.end(), greater<int>()); // ½«ÀÛ¼Ó¾ØÕó´Ó´óµ½Ð¡½øÐÐÅÅÐò

	while (1) {
		int weight = sortCircleWeight[count], index;
		vector<int>::iterator iter = find(circleWeight.begin(), circleWeight.end(), weight);
		index = iter - circleWeight.begin();
		int a = circles[index].first, b = circles[index].second;
		count++;

		int i;
		for (i = 0; i < center.size(); i++) {
			if (sqrt(pow((center[i].first - a), 2) + pow((center[i].second - b), 2)) < minRadius) {
				break; // ÅÐ¶Ï¼ì²â³öÀ´µÄÔ²ÐÄ×ø±êÊÇ·ñ¸úÒÑ¼ì²âµÄÔ²ÐÄ×ø±êµÄ¾àÀë£¬Èç¹û¾àÀë¹ýÐ¡£¬Ä¬ÈÏÊÇÍ¬¸öÔ²
			}
		}
		if (i == center.size()) {
			center.push_back(make_pair(a, b));
			outputImage.draw_circle(a, b, r, red, 5.0f, 1);
			break;
		}
	}		
}