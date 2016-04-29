#include "Basic.h"
#include "Detector.h"


using namespace qr;

int main()
{
	Image img("qr_code.bmp");
	Detector detector(img);
	detector.detect();
	

	return 0;
}
