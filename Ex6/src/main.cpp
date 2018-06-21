#include "ImageStitching.h"

int main() {
	// read image filenames
	string filepath = "../TEST-ImageData(2)/";
	string outputpath = "../result/";
	vector<string> filenames;
	
	// under ubuntu environment
	DIR *dir;
	struct dirent *ptr;
	if ((dir=opendir(filepath.c_str())) == NULL) {
		perror("open directory error!");
		exit(1);
	}
	while ((ptr=readdir(dir)) != NULL) {
		if(ptr->d_type == 8) 
			filenames.push_back(ptr->d_name);
		else 
			continue;
	}
	closedir(dir);
	
	// run image stitching
	ImageStitching stitch;
	stitch.run(filepath, filenames);
}
