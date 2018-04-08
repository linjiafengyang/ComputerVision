#include "canny.cpp"

int main() {
	CImg<unsigned char> bigben_grey("../test_Data/bigben.bmp");
	Canny c;
	CImg<unsigned char> bigben_canny = c.canny(bigben_grey, bigben_grey.width(), bigben_grey.height());
	bigben_canny.save("../result_Data/bigben_canny.bmp");

	CImg<unsigned char> lena_grey("../test_Data/lena.bmp");
	CImg<unsigned char> lena_canny = c.canny(lena_grey, lena_grey.width(), lena_grey.height());
	lena_canny.save("../result_Data/lena_canny.bmp");
	//lena_canny.display("lena_canny");

	CImg<unsigned char> stpietro_grey("../test_Data/stpietro.bmp");
	CImg<unsigned char> stpietro_canny = c.canny(stpietro_grey, stpietro_grey.width(), stpietro_grey.height());
	stpietro_canny.save("../result_Data/stpietro_canny.bmp");

	CImg<unsigned char> twows_grey("../test_Data/twows.bmp");
	CImg<unsigned char> twows_canny = c.canny(twows_grey, twows_grey.width(), twows_grey.height());
	twows_canny.save("../result_Data/twows_canny.bmp");
	return 0;
}