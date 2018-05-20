#include "iteration.cpp"

int main() {
	for (int i = 1; i <= 100; i++) {
		string filename = "../dataset/" + to_string(i) + ".bmp";
		CImg<float> src(filename.c_str());
		Iteration iter;
		CImg<float> result = iter.run_iteration(src);
		string resultname = "../result_iteration/" + to_string(i) + ".bmp";
		result.save(resultname.c_str());
	}
	return 0;
}