import sys
import cv2 as cv
import numpy as np
def main(argv):
    
    default_file = "2.bmp"
    filename = argv[0] if len(argv) > 0 else default_file
    # Loads an image
    src = cv.imread(filename, cv.IMREAD_COLOR)
    # Check if image is loaded fine
    if src is None:
        print ('Error opening image!')
        print ('Usage: hough_circle.py [image_name -- default ' + default_file + '] \n')
        return -1
    
    gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    
    gray = cv.medianBlur(gray, 5)
    
    rows = gray.shape[0]
    """
    各张图的最小半径minRadius和最大半径maxRadius
    1.bmp: 150 230
    2.bmp: 190 230
    3.bmp: 146 230
    4.bmp: 146 200
    5.bmp: 480 525
    6.bmp: 40 60
    """
    circles = cv.HoughCircles(gray, cv.HOUGH_GRADIENT, 1, rows / 8,
                               param1=100, param2=30,
                               minRadius=40, maxRadius=60)
    
    if circles is not None:
        circles = np.uint16(np.around(circles))
        print("硬币数量：", circles.shape[1])
        for i in circles[0, :]:
            center = (i[0], i[1])
            print("圆心：", center)
            # circle center
            cv.circle(src, center, 1, (0, 0, 255), 6)
            # circle outline
            radius = i[2]
            print("半径：", radius)
            cv.circle(src, center, radius, (0, 0, 255), 3)
    
    cv.imshow("detected circles", src)
    cv.imwrite("result/" + filename, src);
    cv.waitKey(0)
    
    return 0
if __name__ == "__main__":
    main(sys.argv[1:])