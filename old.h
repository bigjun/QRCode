#pragma once
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

using namespace cv;
using namespace std;

namespace old_version
{
const double PI = 3.141592653;

typedef pair<int, int> My_Point;

double my_distance(My_Point p1, My_Point p2);

vector<My_Point> my_make_point_unique(const vector<My_Point>& many);

// 给定一个列块，找到11311的组合，并且把中点（row）返回
vector<int> my_col_check_ratio(const vector<pair<int, int>>& col_count);

// 给定一个行块，找到11311的组合，并且把中点（col）返回
vector<int> my_row_check_ratio(const vector<pair<int, int>>& row_count);

// 返回一列的块个数的vector
vector<pair<int, int>> my_col_count(const Mat& img_bw, int col);
// 返回一行的块个数的vector
vector<pair<int, int>> my_row_count(const Mat& img_bw, int row);

vector<pair<int, int>> my_localization(const Mat& img_gray);

Mat my_rgb2gray(const Mat& img_rgb);

Mat my_medfilter(const Mat& img_gray);

Mat my_im2bw(const Mat& img_gray);

void my_draw_line_row(Mat& img, int row);

void my_draw_line_col(Mat& img, int col);

Mat my_rotate(cv::Mat img, double angle);

Mat my_qr_rectified(const Mat& img, vector<My_Point> points);

int my_qr_px_size(const Mat& img);

Mat my_qr_cut(const Mat& img, int pw);

Mat my_qr_sampling(const Mat& img, int pw);
Mat my_resize(const Mat& src, int width, int heigh);


cv::Mat createOne(std::vector<cv::Mat> & images, int cols, int min_gap_size);
}
