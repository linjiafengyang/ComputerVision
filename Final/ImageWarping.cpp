#include "ImageWarping.h"

CImg<float> ImageWarping::run(string filename, vector<pair<int,int> > corner) {
	this->srcImg.load(filename.c_str());
	// 读取并标定角点顺序
	this->sort_corner = SortCorner(corner);
	// 判断方向
	// 0为Vertical，1为Horizontal
	if (this->direction == 0) 
		this->result = CImg<float>(420, 594, 1, 3, 0);
	else 
		this->result = CImg<float>(594, 420, 1, 3, 0);
	// 计算投影矩阵
	this->trans_matrix = ComputeMatrix(this->sort_corner);
	// 投影
	cimg_forXY(this->result, x, y) {
		pair<int, int> point = Transform(this->trans_matrix, make_pair(x,y));
		int u = point.first;
		int v = point.second;
		this->result._atXY(x,y,0,0) = this->srcImg._atXY(u,v,0,0);
		this->result._atXY(x,y,0,1) = this->srcImg._atXY(u,v,0,1);
		this->result._atXY(x,y,0,2) = this->srcImg._atXY(u,v,0,2);
	}
	return this->result;
}
vector<pair<int,int> > ImageWarping::SortCorner(vector<pair<int,int> >& corner) {
	vector<pair<int,int> > result(4);
	int center_x = 0, center_y = 0;
	for (int i = 0 ; i < corner.size(); ++i) {
		center_x += corner[i].first;
		center_y += corner[i].second;
	}
	center_x /= corner.size();
	center_y /= corner.size();
	int count = 0;
	for (int i = 0 ; i < corner.size(); ++i) {
		if(corner[i].first <= center_x && corner[i].second <= center_y) 
			count++;
	}
	if (count == 1) {
		for (int i = 0 ; i < corner.size(); ++i) {
			if (corner[i].first <= center_x && corner[i].second <= center_y)
				result[0] = corner[i];
			else if (corner[i].first <= center_x && corner[i].second >= center_y)
				result[1] = corner[i];
			else if (corner[i].first >= center_x && corner[i].second >= center_y)
				result[2] = corner[i];
			else if (corner[i].first >= center_x && corner[i].second <= center_y)
				result[3] = corner[i];
		}
		int delta_x = abs(result[0].first - center_x);
		int delta_y = abs(result[0].second - center_y);
		this->direction = delta_x < delta_y ? 0 : 1;
	}
	else if (count == 2) {
		vector<pair<int,int> > left;
		vector<pair<int,int> > right;
		for (int i = 0 ; i < corner.size(); ++i) {
			if (corner[i].first <= center_x && corner[i].second <= center_y) 
				left.push_back(corner[i]);
			else if (corner[i].first >= center_x && corner[i].second >= center_y)
				right.push_back(corner[i]);
		}
		result[0] = left[0].first > left[1].first ? left[0] : left[1];
		result[1] = left[0].first < left[1].first ? left[0] : left[1];
		result[2] = right[0].first < right[1].first ? right[0] : right[1];
		result[3] = right[0].first > right[1].first ? right[0] : right[1];
		int delta_x = abs(result[0].first - center_x);
		int delta_y = abs(result[0].second - center_y);
		this->direction = delta_x > delta_y ? 0 : 1;
	}
	return result;
}

vector<float> ImageWarping::ComputeMatrix(vector<pair<int,int> > uv) {
	// 初始化投影坐标
	vector<pair<int, int> > xy;
	xy.push_back(make_pair(0,0));
	xy.push_back(make_pair(0,this->result._height));
	xy.push_back(make_pair(this->result._width,this->result._height));
	xy.push_back(make_pair(this->result._width,0));
	//get the 8 point
	float u1 = uv[0].first, v1 = uv[0].second;
	float u2 = uv[1].first, v2 = uv[1].second;
	float u3 = uv[2].first, v3 = uv[2].second;
	float u4 = uv[3].first, v4 = uv[3].second;
	float x1 = xy[0].first, y1 = xy[0].second;
	float x2 = xy[1].first, y2 = xy[1].second;
	float x3 = xy[2].first, y3 = xy[2].second;
	float x4 = xy[3].first, y4 = xy[3].second;

	float A[8][9] = {
					{x1, y1, 1, 0, 0, 0, -u1*x1, -u1*y1, u1},
					{0, 0, 0, x1, y1, 1, -v1*x1, -v1*y1, v1},
					{x2, y2, 1, 0, 0, 0, -u2*x2, -u2*y2, u2},
					{0, 0, 0, x2, y2, 1, -v2*x2, -v2*y2, v2},
					{x3, y3, 1, 0, 0, 0, -u3*x3, -u3*y3, u3},
					{0, 0, 0, x3, y3, 1, -v3*x3, -v3*y3, v3},
					{x4, y4, 1, 0, 0, 0, -u4*x4, -u4*y4, u4},
					{0, 0, 0, x4, y4, 1, -v4*x4, -v4*y4, v4},
				  };

	if (A[0][0] == 0) {
		for (int i = 1; i < 8; i++) {
			if (A[i][0] != 0) {
				//swap the row and break
				float temp;
				for (int j = 0; j < 9; j++) {
					temp = A[0][j];
					A[0][j] = A[i][j];
					A[i][j] = temp;
				}
				break;
			}
		}
	}

	for (int i = 1; i < 8; i++) {
		float max = 0;
		int index;
		for (int j = i-1; j < 8; j++) {
			if (abs(A[j][i-1]) > max) {
				max = abs(A[j][i-1]);
				index = j;
			}
		}
		for (int j = 0; j < 9; j++) {
			float temp = A[i-1][j];
			A[i-1][j] = A[index][j];
			A[index][j] = temp;
		}
		for (int j = i; j < 8; j++) {
			float x = A[j][i-1] / A[i-1][i-1];
			for (int k = i-1; k < 9; k++) {
				A[j][k] = A[j][k] - x*A[i-1][k];
			}
		}
		if (A[i][i] == 0) {
			for (int j = i+1; j < 8; j++) {
				if (A[j][i] != 0) {
					float temp;
					for (int k = 0; k < 9; k++) {
						temp = A[i][k];
						A[i][k] = A[j][k];
						A[j][k] = temp;
					}
					break;
				}
			}
		}
	}
	vector<float> result(8);
	for (int i = 7; i >= 0; i--) {
		float b = A[i][8];
		for (int j = 7; j >= i+1; j--)
			b = b - A[i][j] * result[j];
		result[i] = b/A[i][i];
	}
	return result;
}

pair<int, int> ImageWarping::Transform(vector<float> matrix, pair<int,int> point)
{
	int u = point.first;
	int v = point.second;
	float q = matrix[6]*u + matrix[7]*v + 1;
	float x = (matrix[0]*u + matrix[1]*v + matrix[2])/q;
	float y = (matrix[3]*u + matrix[4]*v + matrix[5])/q;
	// 四舍五入，最近邻
	return pair<int, int>((int)x+0.5f, (int)y+0.5f);
}