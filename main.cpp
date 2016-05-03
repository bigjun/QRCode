#include "Basic.h"
#include "Detector.h"

using namespace qr;

int main()
{
    //Image img("qr_code.bmp");
    Image img("liantu.png");
    Detector detector(img);
    detector.detect();
    

    return 0;
}