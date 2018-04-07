#include "CImg.h"
using namespace cimg_library;

int main() {
    CImg<unsigned char> image("1.bmp");
    image.display("Ex1.1");
    return 0;
}
