#include "CImg.h"
#include <cmath>
using namespace cimg_library;

int main() {
    CImg<unsigned char> image(200, 200, 1, 3, 0);
    //image.fill(0);
    cimg_forXY(image, x, y) {
        double x1 = pow(x - 50, 2);
        double y1 = pow(y - 50, 2);
        double distance = sqrt(x1 + y1);
        // 小于圆的半径3，即在圆的内部，填充黄色
        if (distance <= 3.0) {
            image(x, y, 0) = 255;
            image(x, y, 1) = 255;
            image(x, y, 2) = 0;
        }
    }
    image.display("Ex1.4");
    return 0;
}