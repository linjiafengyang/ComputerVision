#include "EdgeDetect.cpp"
#include "CANNY.cpp"
#include <cstdlib>

int main() {
	EdgeDetect *edgeDetect1 = new EdgeDetect("Dataset/Dataset1/1.bmp", "result/result1/1.bmp", "line", 4);
	EdgeDetect *edgeDetect2 = new EdgeDetect("Dataset/Dataset1/2.bmp", "result/result1/2.bmp", "line", 4);
	EdgeDetect *edgeDetect3 = new EdgeDetect("Dataset/Dataset1/3.bmp", "result/result1/3.bmp", "line", 4);
	EdgeDetect *edgeDetect4 = new EdgeDetect("Dataset/Dataset1/4.bmp", "result/result1/4.bmp", "line", 6);
	EdgeDetect *edgeDetect5 = new EdgeDetect("Dataset/Dataset1/5.bmp", "result/result1/5.bmp", "line", 4);
	EdgeDetect *edgeDetect6 = new EdgeDetect("Dataset/Dataset1/6.bmp", "result/result1/6.bmp", "line", 4);
	EdgeDetect *edgeDetect7 = new EdgeDetect("Dataset/Dataset2/1.bmp", "result/result2/1.bmp", "circle", 1, 150, 170);
	EdgeDetect *edgeDetect8 = new EdgeDetect("Dataset/Dataset2/2.bmp", "result/result2/2.bmp", "circle", 4, 180, 250);
	EdgeDetect *edgeDetect9 = new EdgeDetect("Dataset/Dataset2/3.bmp", "result/result2/3.bmp", "circle", 7, 120, 200);
	EdgeDetect *edgeDetect10 = new EdgeDetect("Dataset/Dataset2/4.bmp", "result/result2/4.bmp", "circle", 3, 150, 250);
	EdgeDetect *edgeDetect11 = new EdgeDetect("Dataset/Dataset2/5.bmp", "result/result2/5.bmp", "circle", 2, 420, 540);
	EdgeDetect *edgeDetect12 = new EdgeDetect("Dataset/Dataset2/6.bmp", "result/result2/6.bmp", "circle", 5, 40, 70);
	system("pause");
	return 0;
}