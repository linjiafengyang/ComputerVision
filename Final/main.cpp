#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <direct.h>
#include "Process.h"
#include "Canny.h"
#include "Hough.h"
#include "ImageWarping.h"
#include "ImageSegmentation.h"
using namespace std;

vector<string> getFiles(string cate_dir) {
    // 存放文件名
    vector<string> files;

#ifdef WIN32
    _finddata_t file;
    long lf;
    // 输入文件夹路径
    if ((lf=_findfirst(cate_dir.append("\\*").c_str(), &file)) == -1L) {
        cout<<cate_dir<<" not found!!!"<<endl;
    } else {
        while(_findnext(lf, &file) == 0) {
            // 输出文件名
            if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
                continue;
            files.push_back(file.name);
        }
    }
    _findclose(lf);
#endif

#ifdef linux
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(cate_dir.c_str())) == NULL)
        {
        perror("Open dir error...");
                exit(1);
        }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
                continue;
        else if(ptr->d_type == 8)    ///file
            files.push_back(ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            continue;
        else if(ptr->d_type == 4)    ///dir
        {
            files.push_back(ptr->d_name);
        }
    }
    closedir(dir);
#endif
    // 排序，按从小到大排序
    sort(files.begin(), files.end());
    return files;
}
// 生成存储图片的文件夹
void createFolders() {
	if (_access("result", 0) == -1)
		_mkdir("result");
	if (_access("result/warping", 0) == -1)
		_mkdir("result/warping");
	if (_access("result/singleNumImg", 0) == -1)
		_mkdir("result/singleNumImg");
}
int main() {
    createFolders();
	string filepath = "./testdata/"; // 图像文件夹
	vector<string> filenames = getFiles(filepath); // 读取图像文件名
    //ofstream out("imageDir.txt", ios::app);
	for (int i = 0; i < filenames.size(); i++) {
		string filename = filepath + filenames[i];
		cout << filename << endl;
        // 预处理
        Process p;
        CImg<float> grayImage = p.preprocess(filename);
        grayImage.display("Gray Image");
        int scale = p.getScale();
        // Canny边缘检测
        Canny c;
        CImg<float> canny_result = c.run(grayImage);
        canny_result.display("Canny");
        // 霍夫变换
        Hough h;
        CImg<float> hough_result = h.run(canny_result, scale);
        hough_result.display("Hough");
        vector<pair<int, int> > corner = h.getCorner();
        // A4纸校正
        ImageWarping warp;
        CImg<float> warping_result = warp.run(filename, corner);
        warping_result.display("Warping");
        string warpingpath = "./result/warping/"; // 存放校正后的图像 
        warpingpath = warpingpath + filenames[i];
        warping_result.save(warpingpath.c_str());

        // 存储文件夹路径，用于python读取后进行识别工作
        string sni = "result/singleNumImg/";
        sni = sni + filenames[i].substr(0,8);
        // if (out.is_open()) {
        //     out << sni << endl;
        // }
        // 数字字符切割
        ImageSegmentation numberSegmentation;
        numberSegmentation.run(warping_result, sni.c_str());
	}
    //out.close();
    return 0;
}
