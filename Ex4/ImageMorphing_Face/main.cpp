#include "ImageMorphing.cpp"
#include <fstream>
#include <sstream>
#include <string>
int main() {
	CImg<float> src, dst;
	src.load_bmp("dataset/1.bmp");
	dst.load_bmp("dataset/2.bmp");

	// read source points
	vector<Point> src_points;
	ifstream infile_src_points;
	infile_src_points.open("points/src_points.txt");
	if (infile_src_points.is_open()) {
		string s;
		int x, y;
		while (getline(infile_src_points, s)) {
			stringstream ss(s);
			ss >> x >> y;
			Point p(x, y);
			src_points.push_back(p);
		}
	}
	else {
		cout << "Can't open the file src_points.txt" << endl;
		return 0;
	}

	// read destination points
	vector<Point> dst_points;
	ifstream infile_dst_points;
	infile_dst_points.open("points/dst_points.txt");
	if (infile_dst_points.is_open()) {
		string s;
		int x, y;
		while (getline(infile_dst_points, s)) {
			stringstream ss(s);
			ss >> x >> y;
			Point p(x, y);
			dst_points.push_back(p);
		}
	}
	else {
		cout << "Can't open the file dst_points.txt" << endl;
		return 0;
	}

	// read the indexs of triangles points
	vector<vector<int> > index;
	ifstream infile_tri;
	infile_tri.open("points/triangle.txt");
	if (infile_tri.is_open()) {
		string s;
		int index1, index2, index3;
		while (getline(infile_tri, s)) {
			vector<int> tempindex;
			stringstream ss(s);
			ss >> index1 >> index2 >> index3;
			tempindex.push_back(index1 - 1);
			tempindex.push_back(index2 - 1);
			tempindex.push_back(index3 - 1);
			index.push_back(tempindex);
		}
	}
	else {
		cout << "Can't open the file triangle.txt" << endl;
		return 0;
	}

	ImageMorphing imageMorphing(src, dst, src_points, dst_points, index, 11); // 11帧
	CImgList<float> result = imageMorphing.morphing();
	string dir_name;
	for (int i = 0; i < result.size(); i++) {
		dir_name = "result/";
		string s = to_string(i+1);
		s += ".bmp";
		dir_name += s;
		result[i].save_bmp(dir_name.c_str()); // Save every frame
	}
	return 0;
}