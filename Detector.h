#pragma once
#include "Basic.h"
#include <iostream>

namespace qr
{
using cv::Mat;
using cv::Point2i;
using cv::Scalar;
using std::cout;
using std::endl;
using std::max;
using std::vector;
using std::pair;
using std::make_pair;
typedef pair<int, int> My_Point;
const double PI = 3.141592653;

class Detector
{
public:
    Detector(Image& image) :img_(image) {}
    void detect();
    //Mat rectified();
    void findPositionDetectionPattern();
    Mat rectified();
    Mat cut();
    int nominalX();

private:
    Image& img_;
    
    // A ... B
    //
    // C
    My_Point A_; // left top
    My_Point B_; // right top
    My_Point C_; // left bottom
};

}