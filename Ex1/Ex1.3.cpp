#include "CImg.h"
#include <cmath>
using namespace cimg_library;

int main() {
    // CImg<unsigned char> image("1.bmp");
    CImg<unsigned char> image(200, 200, 1, 3);
    image.fill(0);// 填充0，为黑色
    cimg_forXY(image, x, y) {
        double x1 = pow(x - 50, 2);
        double y1 = pow(y - 50, 2);
        double distance = sqrt(x1 + y1);
        // 小于圆的半径30，即在圆的内部，填充蓝色
        if (distance <= 30.0) {
            image(x, y, 0) = 0;
            image(x, y, 1) = 0;
            image(x, y, 2) = 255;
        }
    }
    image.display("Ex1.3");
    return 0;
}