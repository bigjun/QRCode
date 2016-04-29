#pragma once
#include "Basic.h"

namespace qr
{
class Detector
{
public:
	Detector(Image& image) :img(image) {}
	void detect()
	{
		ImageDisplay displayer(img);
		displayer.display();
		ImageProc processor(img);
		processor.rgb2gray();
		displayer.display();
		processor.filter();
		displayer.display();
		processor.binaryzation();
		displayer.display();
	}

private:
	// 找到三个 Position Detection Pattern
	void findPositionDetectionPattern();
	void sampling();
	Image& img;
	
	// LT ... RT
	//
	// LB
	cv::Point2i LT; // left top
	cv::Point2i RT; // right top
	cv::Point2i LB; // left bottom
};

}