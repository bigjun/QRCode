#include "Basic.h"
#include "Detector.h"

using namespace qr;

int main()
{
    //Image img("qr_code.bmp");
    //Image img("liantu.png");
    //Image img("standard.png");
    Image img("qr3.png");
    Detector detector(img);
    detector.detect();
    cout << (!0)*255 << endl;
    cout << (!(!0) * 255) * 255 << endl;

    return 0;
}