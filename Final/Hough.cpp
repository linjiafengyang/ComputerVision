#include "Hough.h"

CImg<float> Hough::run(CImg<float> thres, int scale) {
	this->thres = thres;
	for (int j = 0 ; j < theta_size; ++j) {
		this->tabSin.push_back(sin(cimg::PI*j/(theta_size)));
		this->tabCos.push_back(cos(cimg::PI*j/(theta_size)));
	}
	houghLinesTransform(thres);
	houghLinesDetect();
	findEdge();
	findPoint(scale);
	return this->edgeImage;
}
// 映射到霍夫空间
void Hough::houghLinesTransform(CImg<float> &imgin) {
	int width = imgin._width;
	int height = imgin._height;
	int max_length = sqrt((width/2)*(width/2) + (height/2)*(height/2));
	int rows = theta_size;
	int cols = max_length*2;
	this->houghspace = CImg<float>(cols, rows);
	this->houghspace.fill(0);

	cimg_forXY(imgin, x, y) {
		int p = imgin._atXY(x,y);
		if (p == 0)
			continue;
		int x0 = x - width/2;
		int y0 = height/2 - y;
		for (int i = 0 ; i < theta_size; ++i) {
			int r = int(x0 * tabCos[i] + y0 * tabSin[i] ) + max_length;

			if (r < 0 || r >=  max_length*2)
				continue;
			this->houghspace._atXY(r,i) += 1;
		}
	}
}

bool compare(pair<int,int> a,pair<int,int> b) {
	return a.second > b.second;
}
void Hough::houghLinesDetect() {
	int width = this->houghspace._width;
	int height = this->houghspace._height;

	// 霍夫空间取极大值点
	int window_size = 40;
	for (int i = 0; i < height; i += window_size/2) {
		for (int j = 0; j < width; j += window_size/2) {
			int max = getMaxValue(this->houghspace, window_size, i, j);
			int y_max = i + window_size < height ? i + window_size : height;
			int x_max = j + window_size < width ? j + window_size : width;
			bool is_max = true;
			for ( int y = i; y < i + window_size; ++y) {
				for (int x = j; x < j + window_size; ++x) {
					if (this->houghspace._atXY(x,y) < max)
						this->houghspace._atXY(x,y) = 0;
					else if (!is_max)
						this->houghspace._atXY(x,y) = 0;
					else
						is_max = false;
				}
			}
		}
	}

	// 所有的极大值点保存到line数组中
	cimg_forXY(this->houghspace,x,y) {
		if (this->houghspace._atXY(x,y)>0)
			this->lines.push_back(make_pair(y*width+x, this->houghspace._atXY(x,y)));
	}
	// 根据权重从大到小排序
	sort(this->lines.begin(), this->lines.end(), compare);
}

// 获取一个窗口内最大权重
int Hough::getMaxValue(CImg<float> &img, int &size, int &y, int &x) {
	int max = 0;
	int width = x+size > img._width ? img._width : x+size;
	int height = y + size > img._height ? img._height : y + size;
	for (int j = x; j < width; ++j) {
		for (int i = y ; i < height; ++i) {
			if (img._atXY(j,i) > max)
				max = img._atXY(j,i);
		}
	}
	return max;
}
void Hough::findEdge() {
	int max_length = this->houghspace._width / 2;

	this->edgeImage = CImg<float>(this->thres._width, this->thres._height, 1, 1, 0);
	// 取前point_num条边
	for (int i = 0; i < point_num; ++i) {
		int n = this->lines[i].first;
		int theta = n / this->houghspace._width;
		int r = n % this->houghspace._width - max_length;
		this->edge.push_back(make_pair(theta,r));
		// cout << "theta:" << (theta*1.0/500)*180 << " r:" << r << " weight:" << this->lines[i].second << endl;
		for (int x = 0; x < this->thres._width; ++x) {
			for (int y = 0; y < this->thres._height; ++y) {
				int x0 = x - this->thres._width/2 ;
				int y0 = this->thres._height/2 - y ;
				if(r == int(x0 * tabCos[theta] + y0 * tabSin[theta]))
					this->edgeImage._atXY(x,y) += 255.0/2;
			}
		}
	}

}
void Hough::findPoint(int scale) {
	int width = this->thres._width;
	int height = this->thres._height;
	int max_length = this->houghspace._width / 2;
	int n1,n2,r1,r2,theta1,theta2;
	double x,y;
	unsigned char red[3] = {255,0,0};
	for (int i = 0; i < point_num; ++i) {
		for(int j = i+1; j < point_num; ++j) {
			r1 = this->edge[i].second;
			r2 = this->edge[j].second;
			theta1 =this->edge[i].first;
			theta2 =this->edge[j].first;
			if (abs(theta1-theta2) < 40)
				continue;
			y = (r2)*1.0/tabCos[int(theta2)] - (r1)*1.0/tabCos[int(theta1)];
			y = y*1.0/(tabSin[int(theta2)]/tabCos[int(theta2)] - tabSin[int(theta1)]/tabCos[int(theta1)]);
			x = r1/tabCos[int(theta1)] - y*tabSin[int(theta1)]/tabCos[int(theta1)];
			if ((x+width/2) > 0 && (x+width)/2 < width && (height/2-y) > 0 && (height/2-y) < height) {
				cout << "(x, y) = (" << (x+width/2) << ", "  << (height/2-y) << ")" << endl;
				this->corner.push_back(make_pair(int(scale*(x+width/2)),int(scale*(height/2-y))));
			}
		}
	}
}

vector<pair<int, int> > Hough::getCorner() {
	return this->corner;
}