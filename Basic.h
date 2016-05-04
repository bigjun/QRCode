#pragma once
// OpenCV 基本函数的封装
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> // imread
#include <opencv2/imgproc/imgproc.hpp> //cvtColor
#include <string>
#include "old.h"


namespace qr
{

class Image
{
public:
    Image(const std::string filename)
    {
        raw_ = cv::imread(filename.c_str());
    }
    Image(cv::Mat& image) :raw_(image) {};
    void reassign(Mat image)
    {
        image.copyTo(raw_);
    }
    cv::Mat& mat()
    {
        return raw_;
    }
    void write(const std::string filename);
private:
    cv::Mat raw_;
};

class ImageDisplay
{
public:
    ImageDisplay(Image& image) :img_(image) {}
    void display()
    {
        std::string w_0("Window");
        cv::namedWindow(w_0, cv::WINDOW_NORMAL);
        cv::imshow(w_0, img_.mat());
        cv::waitKey();
    }
private:
    Image& img_;
};


class ImageProc
{
public:
    ImageProc(Image& image):img_(image) {}
    void rgb2gray() 
    {
        cv::cvtColor(img_.mat(), img_.mat(), cv::COLOR_BGR2GRAY);
    }
    void filter()
    {
        img_.reassign(old_version::my_medfilter(img_.mat()));
        imwrite("filtered.bmp", img_.mat());
        //cv::medianBlur(img_.mat(), img_.mat(), 7);
    }
    void binaryzation()
    {
        img_.reassign(old_version::my_im2bw(img_.mat()));
        imwrite("binaryzation.bmp", img_.mat());
        //int blockSize = 35;
        //int C = 10;
        //cv::threshold(img_.mat(), img_.mat(), 127, 255, cv::THRESH_OTSU);
        //cv::adaptiveThreshold(img.mat(), img.mat(), 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, blockSize, C);
    }
    void draw();
    void rotate(cv::Point2f p, double angle, double scale = 1)
    {
        int s = static_cast<int>(cv::sqrt(img_.mat().rows*img_.mat().rows + img_.mat().cols*img_.mat().cols));
        cv::warpAffine(img_.mat(), img_.mat(), cv::getRotationMatrix2D(p, angle, scale), cv::Size(s, s));
    }
private:
    Image& img_;
};

}