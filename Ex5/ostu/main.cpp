#include "ostu.cpp"

int main() {
	for (int i = 1; i <= 100; i++) {
		string filename = "../dataset/" + to_string(i) + ".bmp";
		CImg<float> src(filename.c_str());
		OSTU o;
		CImg<float> result = o.run_ostu(src);
		//result.display();
		string resultname = "../result_ostu/" + to_string(i) + ".bmp";
		result.save(resultname.c_str());
	}
	return 0;
}