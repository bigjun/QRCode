#include "Detector.h"
#include <vector>
#include <iostream>
#include <utility> // pair
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> // resize
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric> // accumulate
#include <cassert>
#include <utility>
#include <cmath>
#include <map>
#include <cstdlib>
#include "old.h"

using namespace cv;
using namespace std;
using namespace qr;
using namespace old_version;

void Detector::findPositionDetectionPattern()
{
    auto points = old_version::my_localization(img_.mat());
    assert(points.size() >= 3);

    My_Point A = points[0];
    My_Point B = points[1];
    My_Point C = points[2];
    double AB = my_distance(A, B);
    double AC = my_distance(A, C);
    double BC = my_distance(B, C);
    My_Point left_top;
    My_Point left_bottom;
    My_Point right_top;
    if (AB > AC && AB > BC)
    {
        left_top = C;
        if (abs(C.second - B.second) < abs(C.second - A.second))
        {
            left_bottom = A;
            right_top = B;
        }
        else
        {
            left_bottom = B;
            right_top = A;
        }

    }
    else if (AC > AB && AC > BC)
    {
        left_top = B;
        if (abs(B.second - A.second) < abs(B.second - C.second))
        {
            left_bottom = C;
            right_top = A;
        }
        else
        {
            left_bottom = A;
            right_top = C;
        }
    }
    else if (BC > AB && BC > AC)
    {
        left_top = A;
        if (abs(A.second - B.second) < abs(A.second - C.second))
        {
            left_bottom = C;
            right_top = B;
        }
        else
        {
            left_bottom = B;
            right_top = C;
        }
    }
    A_ = left_top;
    B_ = right_top;
    C_ = left_bottom;
    printf("A:(%d,%d), B(%d,%d), C(%d,%d)\n", A_.first, A_.second, B_.first, B_.second, C_.first, C_.second);
}

void Detector::rectified()
{
    img_ = old_version::my_qr_rectified(img_.mat(), vector<My_Point>{A_, B_, C_});
}

int Detector::calcSquareSize(qr::My_Point& p)
{
    cout << "calc square size" << " p is " << "("<<p.first<<", "<<p.second<<")" << endl;
    Mat& img = img_.mat();
    int x = p.first;
    int y = p.second;
    int L = 0;
    int R = 0;
    assert(img.at<uchar>(x, y) == 0); // center of square is black

    int pre = 0;
    int cur = y;
    for (int i = 0; i < 3;)
    {
        
        while (cur > 0 && cur < img.cols && i < 3)
        {
            //cout << "cur:" << cur << " i: " << i << endl;
            if (img.at<uchar>(x, cur) == pre)
            {
                cout << "same " << "cur" << cur << " " << " L " << L << endl;
                cur--;
                L++;
            }
            else
            {
                cout << "not same " << "cur" << cur << " " << " L " << L << endl;
                cout << "i=" << i << " L=" << L << endl;
                i++; // three times change
                cur--; // coordinate
                L++; 
                pre = (!pre) * 255;
            }
        }
    }
    pre = 0;
    cur = y;
    for (int i = 0; i < 3;)
    {
        while (cur > 0 && cur < img.cols && i < 3)
        {
            if (img.at<uchar>(x, cur) == pre)
            {
                cur++;
                R++;
            }
            else
            {
                i++; // three times change
                cur++; // coordinate
                R++;
                pre = (!pre) * 255;
            }
        }
    }
    cout << "L:" << L << " " << "R: " << R << endl;

    int T = 0;
    int B = 0;
    pre = 0;
    cur = x;
    for (int i = 0; i < 3;)
    {
        while (cur > 0 && cur < img.rows && i < 3)
        {
            if (img.at<uchar>(cur, y) == pre)
            {
                cur--;
                T++;
            }
            else
            {
                i++; // three times change
                cur--; // coordinate
                T++;
                pre = (!pre) * 255;
            }
        }
    }
    pre = 0;
    cur = x;
    for (int i = 0; i < 3;)
    {
        while (cur > 0 && cur < img.cols && i < 3)
        {
            if (img.at<uchar>(cur, y) == pre)
            {
                cur++;
                B++;
            }
            else
            {
                i++; // three times change
                cur++; // coordinate
                B++;
                pre = (!pre) * 255;
            }
        }
    }
    cout << "T:" << T << " " << "B: " << B << endl;

    p.first = (p.first - L) + (L + R + 1) / 2 + 0.5;
    p.second = (p.second - T) + (T + B + 1) / 2 + 0.5;
    cout << "P real (" << p.first << ", " << p.second << ")" << endl;
    return ((L + R + 1) + (T + B + 1)) / 2 + 0.5;
}
int Detector::calcSymbolSize()
{
    cout << "calc symbol size" << endl;
    int wul = calcSquareSize(A_);
    int wur = calcSquareSize(B_);
    int wux = calcSquareSize(C_);
    int x = (wul + wur + wux) / 21;
    cout << "Symbol size: " << x << endl;
    symbolSize_ = x;
    return x;
}

Mat Detector::sampling()
{
    cout << "sampling..." << endl;
    Mat& img = img_.mat();
    int size = symbolSize_;
    int startX = A_.first - size*2.5;
    int startY = A_.second - size*2.5;
    int height = img.rows;
    int width = img.cols;
    
    cout << "img width: " << img.rows << endl;
    cout << "img height: " << img.cols << endl;
    int step = size;
    cout << "step: " << step << endl;
    int white = 0;
    int black = 0;

    Mat temp;
    img.copyTo(temp);
    //namedWindow("test", WINDOW_NORMAL);

    Mat result(size, size, CV_8U, Scalar(0));
    for (size_t i = startX; i < height-step; i += step)
    {
        for (size_t j = startY; j < width - step; j += step)
        {
            //cout << "(i, j) " << "(" << i << ", " << j << ")" << endl;
            white = 0;
            black = 0;
            my_draw_line_row(temp, i);
            my_draw_line_col(temp, j);
            for (size_t x = i; x < i + step; ++x)
            {
                for (size_t y = j; y < j + step; ++y)
                {
                    if (x >= 0 && x < width && y >= 0 && y < width)
                    {
                        //cout << "(x, y) " << "(" << x << ", " << y << ")" << endl;
                        auto pixel = img.at<uchar>(x, y);
                        pixel > 0 ? white++ : black++;
                    }
                }
            }
            //cout << "white:black " << white << ":" << black << endl;
            auto v = static_cast<uchar>(white > black ? 255 : 0);

            result.at<uchar>(i / step, j / step) = v;
        }
    }
    imwrite("output\\sampling.bmp", temp);
    //imshow("test", temp);
    return result;
}

void Detector::detect()
{
    ImageDisplay displayer(img_);
    displayer.display();
    ImageProc processor(img_);
    processor.rgb2gray();
    displayer.display();
    processor.filter();
    displayer.display();
    processor.binaryzation();
    displayer.display();
    findPositionDetectionPattern();

    displayer.display();
    rectified();
    imwrite("standard_after.bmp", img_.mat());

    displayer.display();
    findPositionDetectionPattern();
    calcSymbolSize();

    
    //auto m2 = sampling();
    //Image i2(m2);
    //ImageDisplay d3(i2);
    //d3.display();

}