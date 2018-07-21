#include "Canny.h"

CImg<float> Canny::run(CImg<float> grayImage) {
	this->filter = createFilter(gFilterx, gFiltery, sigma);
	CImg<float> gFiltered = useFilter(grayImage, this->filter);
	CImg<float> angles;
	CImg<float> sFiltered = sobel(gFiltered, angles);
	CImg<float> non = nonMaxSupp(sFiltered, angles);
	CImg<float> thres = threshold(non, threshold_min, threshold_max);
	return thres;
}
vector<vector<float> > Canny::createFilter(int row, int column, float sigmaIn) {
	vector<vector<float> > filter;
	for (int i = 0; i < row; i++) {
		vector<float> col;
		for (int j = 0; j < column; j++) {
			col.push_back(-1);
		}
		filter.push_back(col);
	}
	float coordSum = 0;
	float constant = 2.0 * sigmaIn * sigmaIn;
	// Sum is for normalization
	float sum = 0.0;
	for (int x = -row/2; x <= row/2; x++) {
		for (int y = -column/2; y <= column/2; y++) {
			coordSum = (x*x + y*y);
			filter[x + row/2][y + column/2] = (exp(-(coordSum) / constant)) / (cimg::PI * constant);
			sum += filter[x + row/2][y + column/2];
		}
	}
	// Normalize the Filter
	for (int i = 0; i < row; i++)
		for (int j = 0; j < column; j++)
			filter[i][j] /= sum;

	return filter;
}

CImg<float> Canny::useFilter(CImg<float> & img_in, vector<vector<float> >& filterIn) {
	int size = (int)filterIn.size()/2;
	CImg<float> filteredImg(img_in._width, img_in._height, 1, 1);
	filteredImg.fill(0);
	for (int i = size; i < img_in._width - size; i++) {
		for (int j = size; j < img_in._height - size; j++) {
			float sum = 0;
			for (int x = 0; x < filterIn.size(); x++) {
				for (int y = 0; y < filterIn.size(); y++)
					sum += filterIn[x][y] * (float)(img_in._atXY(i + x - size, j + y - size));
			}
			filteredImg._atXY(i, j) = sum;
		}
	}
	return filteredImg;
}

CImg<float> Canny::sobel(CImg<float>& gFiltered, CImg<float>& angles) {
	//Sobel X Filter
	float x1[] = {-1.0, 0, 1.0};
	float x2[] = {-2.0, 0, 2.0};
	float x3[] = {-1.0, 0, 1.0};

	vector<vector<float> > xFilter(3);
	xFilter[0].assign(x1, x1+3);
	xFilter[1].assign(x2, x2+3);
	xFilter[2].assign(x3, x3+3);

	//Sobel Y Filter
	float y1[] = {-1.0, -2.0, -1.0};
	float y2[] = {0, 0, 0};
	float y3[] = {1.0, 2.0, 1.0};

	vector<vector<float> > yFilter(3);
	yFilter[0].assign(y1, y1+3);
	yFilter[1].assign(y2, y2+3);
	yFilter[2].assign(y3, y3+3);

	//Limit Size
	int size = (int)xFilter.size()/2;

	CImg<float> filteredImg(gFiltered._width , gFiltered._height ,1,1);
	filteredImg.fill(0);
	angles = CImg<float>(gFiltered._width , gFiltered._height ,1,1); //AngleMap
	angles.fill(0);
	for (int i = size + gFilterx/2; i < gFiltered._width - size - gFilterx/2 ; i++) {
		for (int j = size + gFilterx/2; j < gFiltered._height - size - gFilterx/2; j++) {
			float sumx = 0;
			float sumy = 0;

			for (int x = 0; x < xFilter.size(); x++) {
				for (int y = 0; y < xFilter.size(); y++) {
					sumx += xFilter[y][x] * (float)(gFiltered._atXY(i + x - size, j + y - size)); //Sobel_X Filter Value
					sumy += yFilter[y][x] * (float)(gFiltered._atXY(i + x - size, j + y - size)); //Sobel_Y Filter Value
				}
			}
			float sumxsq = sumx*sumx;
			float sumysq = sumy*sumy;

			float sq2 = sqrt(sumxsq + sumysq);

			if(sq2 > 255) //Unsigned Char Fix
				sq2 =255;
			filteredImg._atXY(i, j) = sq2;

			if(sumx==0) //Arctan Fix
				angles._atXY(i, j) = 90;
			else
				angles._atXY(i, j) = atan(sumy/sumx) * 180 / cimg::PI;
		}
	}

	return filteredImg;
}

CImg<float> Canny::nonMaxSupp(CImg<float>& sFiltered, CImg<float> & angles) {
	CImg<float> nonMaxSupped(sFiltered._width, sFiltered._height, 1, 1);
	nonMaxSupped.fill(0);
	for (int i = 1; i < sFiltered._width - 1; i++) {
		for (int j = 1; j < sFiltered._height - 1; j++) {
			float Tangent = angles._atXY(i,j);

			nonMaxSupped._atXY(i, j) = sFiltered._atXY(i,j);
			//Horizontal Edge
			if ((-22.5 < Tangent) && (Tangent <= 22.5)) {
				if ((sFiltered._atXY(i,j) < sFiltered._atXY(i,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i,j-1)))
					nonMaxSupped._atXY(i, j) = 0;
			}
			//Vertical Edge
			if ((Tangent <= -67.5) || (67.5 < Tangent)) {
				if ((sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j)))
					nonMaxSupped._atXY(i, j) = 0;
			}

			//-45 Degree Edge
			if ((-67.5 < Tangent) && (Tangent <= -22.5)) {
				if ((sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j-1)))
					nonMaxSupped._atXY(i, j) = 0;
			}

			//45 Degree Edge
			if ((22.5 < Tangent) && (Tangent <= 67.5)) {
				if ((sFiltered._atXY(i,j) < sFiltered._atXY(i+1,j+1)) || (sFiltered._atXY(i,j) < sFiltered._atXY(i-1,j-1)))
					nonMaxSupped._atXY(i, j) = 0;
			}
		}
	}
	return nonMaxSupped;
}

CImg<float> Canny::threshold(CImg<float>& imgin, int low, int high)
{
	if(low > 255) low = 255;
	if(high > 255) high = 255;

	CImg<float> EdgeMat(imgin._width, imgin._height, 1, 1);

	for (int i = 0; i < imgin._width; i++) {
		for (int j = 0; j < imgin._height; j++) {

			EdgeMat._atXY(i,j) = imgin._atXY(i,j);
			if (EdgeMat._atXY(i,j) > high) {
				EdgeMat._atXY(i,j) = 255;
			}
			else if (EdgeMat._atXY(i,j) < low) {
				EdgeMat._atXY(i,j) = 0;
			}
			else {
				bool anyHigh = false;
				bool anyBetween = false;
				for (int x = i-1; x < i+2; x++) {
					for (int y = j-1; y<j+2; y++) {
						if (x <= 0 || y <= 0 || EdgeMat._height || y > EdgeMat._width) {//Out of bounds
							continue;
						}
						else {
							if (EdgeMat._atXY(x,y) > high) {
								EdgeMat._atXY(i,j) = 255;
								anyHigh = true;
								break;
							}
							else if (EdgeMat._atXY(x,y) <= high && EdgeMat._atXY(x,y) >= low) {
								anyBetween = true;
							}
						}
					}
					if(anyHigh)
						break;
				}
				if (!anyHigh && anyBetween) {
					for (int x = i-2; x < i+3; x++) {
						for (int y = j-1; y < j+3; y++) {
							if (x < 0 || y < 0 || x > EdgeMat._height || y > EdgeMat._width) //Out of bounds
								continue;
							else {
								if (EdgeMat._atXY(x,y) > high) {
									EdgeMat._atXY(i,j) = 255;
									anyHigh = true;
									break;
								}
							}
						}
						if(anyHigh)
							break;
					}					
				}
				if(!anyHigh)
					EdgeMat._atXY(i,j) = 0;
			}
		}
	}
	return EdgeMat;
}