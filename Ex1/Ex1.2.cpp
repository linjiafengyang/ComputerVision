#include "CImg.h"
using namespace cimg_library;

int main() {
    CImg<unsigned char> image("1.bmp");
    // cimg_forXY函数相当于对x和y循环遍历
    cimg_forXY(image, x, y) {
        // 根据rgb值判断是否为白色，若是则白色区域变成红色
        if (image(x, y, 0) == 255 && image(x, y, 1) == 255 && image(x, y, 2) == 255) {
            image(x, y, 0) = 255;
            image(x, y, 1) = 0;
            image(x, y, 2) = 0;
        }
        // 黑色区域变成绿色
        else if (image(x, y, 0) == 0 && image(x, y, 1) == 0 && image(x, y, 2) == 0) {
            image(x, y, 0) = 0;
            image(x, y, 1) = 255;
            image(x, y, 2) = 0;
        }
    }
    image.display("Ex1.2");
    return 0;
}
