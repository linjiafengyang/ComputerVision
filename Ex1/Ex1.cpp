#include "CImg.h"
using namespace cimg_library;

int main() {
    // 显示1.bmp文件
    CImg<unsigned char> image("1.bmp");
    image.display("Ex1.1");

    // 把1.bmp文件的白色区域变成红色，黑色区域变成绿色
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

    // 在图上绘制一个圆形区域，圆心坐标(50,50)，半径为30，填充颜色为蓝色
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

    // 在图上绘制一个圆形区域，圆心坐标(50,50)，半径为3，填充颜色为黄色
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

    image.display("Ex1");
    return 0;
}
