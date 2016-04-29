#pragma once
// OpenCV 基本函数的封装
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> // imread
#include <opencv2/imgproc/imgproc.hpp> //cvtColor
#include <string>

namespace qr
{

class Image
{
public:
	Image(const std::string filename)
	{
		raw = cv::imread(filename.c_str());
	}
	cv::Mat& mat()
	{
		return raw;
	}
	void write(const std::string filename);
private:
	cv::Mat raw;
};

class ImageDisplay
{
public:
	ImageDisplay(Image& image) :img(image) {}
	void display()
	{
		std::string w_0("Window");
		cv::namedWindow(w_0, cv::WINDOW_NORMAL);
		cv::imshow(w_0, img.mat());
		cv::waitKey();
	}
private:
	Image& img;
};


class ImageProc
{
public:
	ImageProc(Image& image):img(image) {}
	void rgb2gray() 
	{
		cv::cvtColor(img.mat(), img.mat(), cv::COLOR_BGR2GRAY);
	}
	void filter()
	{
		cv::medianBlur(img.mat(), img.mat(), 7);
	}
	void binaryzation()
	{
		int blockSize = 35;
		int C = 10;
		cv::threshold(img.mat(), img.mat(), 127, 255, cv::THRESH_OTSU);
		//cv::adaptiveThreshold(img.mat(), img.mat(), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, C);
	}
	void draw();
	void rotate(cv::Point2f p, double angle, double scale = 1)
	{
		int s = static_cast<int>(cv::sqrt(img.mat().rows*img.mat().rows + img.mat().cols*img.mat().cols));
		cv::warpAffine(img.mat(), img.mat(), cv::getRotationMatrix2D(p, angle, scale), cv::Size(s, s));
	}
private:
	Image& img;
};

}