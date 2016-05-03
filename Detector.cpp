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
    A_ = points[0];
    B_ = points[1];
    C_ = points[2];
}

Mat Detector::rectified()
{
    return old_version::my_qr_rectified(img_.mat(), vector<My_Point>{A_, B_, C_});
}

int Detector::nominalX()
{
    return old_version::my_qr_px_size(img_.mat());
}

Mat Detector::cut()
{
    int w = nominalX();
    return old_version::my_qr_cut(img_.mat(), w);
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
    auto m = rectified();
    Image i(m);
    ImageDisplay d2(i);
    d2.display();
}