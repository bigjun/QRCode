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
#include "common.h"

using namespace cv;
using namespace std;

const double PI = 3.141592653;

typedef pair<int, int> My_Point;

double my_distance(My_Point p1, My_Point p2)
{
	double a = p1.first - p2.first;
	double b = p1.second - p2.second;
	return sqrt(a*a + b*b);
}

vector<My_Point> my_make_point_unique(const vector<My_Point>& many)
{
	const double min_d = 10;
	if (many.size() < 3)
	{
		cout << "无法识别 :-)" << endl;
		exit(-1);
		//throw(runtime_error("point is less than 3"));
	}
	else if (many.size() == 3)
	{
		return many;
	}

	// fix me
	//std::sort(many.begin(), many.end(), [](const My_Point& p1, const My_Point& p2) { return p1.first < p2.first; });
	//std::sort(many.begin(), many.end());
	vector<My_Point> temp;
	temp.push_back(many[0]);
	for (auto p : many)
	{
		bool uniq = true;
		for (auto pp : temp)
		{
			if (my_distance(pp, p) < min_d)
			{
				uniq = false;
			}
		}
		if (uniq)
		{
			temp.push_back(p);
		}
	}
	temp.push_back(many[1]);
	temp.push_back(many[2]);
	for (size_t i = 0; i < 3; i++)//只考虑前三个
	{
		int c = 1;
		for (auto point : many)
		{
			if (my_distance(point, temp[i]) < min_d)
			{
				temp[i].first = (temp[i].first * c + point.first) / (c + 1);
				temp[i].second = (temp[i].second * c + point.second) / (c + 1);
				c++;
			}
		}
	}
	temp.erase(temp.begin() + 3, temp.end());
	cout << "uniq: " << endl;
	for (auto p : temp)
	{
		cout << "Center: " << p.first << ", " << p.second << endl;
	}
	cout << "end uniq" << endl;

	return temp;

}

// 给定一个列块，找到11311的组合，并且把中点（row）返回
vector<int> my_col_check_ratio(const vector<pair<int, int>>& col_count)
{
	size_t len = col_count.size();
	vector<int> temp;
	vector<int> rows;
	for (size_t i = 0; i < len; ++i)
	{
		temp.clear();
		if (len - i >= 5 && col_count[i].second == 0) // first block is black
		{
			for (int j = i; j < i + 5; ++j)
			{
				int x = static_cast<int>(static_cast<double>(col_count[j].first) / static_cast<double>(col_count[i].first) + 0.5);
				temp.push_back(x);
			}
			if (temp[0] == 1 && temp[1] == 1 && temp[2] == 3 && temp[3] == 1 && temp[4] == 1)
			{
				int acc = 0;
				for (int k = 0; k < i + 2; k++)
				{
					acc += col_count[k].first;
				}
				acc += col_count[i + 2].first / 2;
				//cout << acc << "..";
				rows.push_back(acc);
			}
		}
	}
	//cout << endl;
	return rows;
}

// 给定一个行块，找到11311的组合，并且把中点（col）返回
vector<int> my_row_check_ratio(const vector<pair<int, int>>& row_count)
{
	size_t len = row_count.size();
	vector<int> temp;
	vector<int> cols;
	for (size_t i = 0; i < len; ++i)
	{
		temp.clear();
		if (len - i >= 5 && row_count[i].second == 0) // first block is black
		{
			for (size_t j = i; j < i + 5; ++j)
			{
				int x = static_cast<int>(static_cast<double>(row_count[j].first) / static_cast<double>(row_count[i].first) + 0.5);
				temp.push_back(x);
			}
			if (temp[0] == 1 && temp[1] == 1 && temp[2] == 3 && temp[3] == 1 && temp[4] == 1)
			{
				int acc = 0;
				for (int k = 0; k < i + 2; k++)
				{
					acc += row_count[k].first;
				}
				acc += row_count[i + 2].first / 2;
				//cout << acc << "..";
				cols.push_back(acc);
			}
		}
	}
	//cout << endl;
	return cols;
}

// 返回一列的块个数的vector
vector<pair<int, int>> my_col_count(const Mat& img_bw, int col)
{
	vector<pair<int, int>> r;
	int cur = 0;
	int acc = 0;
	int flag = img_bw.at<uchar>(0, col); // flag 0: black 255:white
	for (size_t j = 0; j < img_bw.rows; ++j)
	{
		cur = img_bw.at<uchar>(j, col);
		if (cur == flag)
		{
			acc++;
		}
		else
		{
			r.push_back({ acc, flag });
			flag = flag == 0 ? 255 : 0;
			acc = 1;
		}
	}
	r.push_back({ acc, flag });
	return r;
}

// 返回一行的块个数的vector
vector<pair<int, int>> my_row_count(const Mat& img_bw, int row)
{
	vector<pair<int, int>> r;
	int cur = 0;
	int acc = 0;
	int flag = img_bw.at<uchar>(row, 0); // flag 0: black 255:white
	for (size_t j = 0; j < img_bw.cols; ++j)
	{
		cur = img_bw.at<uchar>(row, j);
		if (cur == flag)
		{
			acc++;
		}
		else
		{
			r.push_back({ acc, flag });
			//cout << acc << ", ";
			flag = flag == 0 ? 255 : 0;
			acc = 1;
		}
	}
	r.push_back({ acc, flag });
	//cout << acc << ", " << endl;
	return r;
}

vector<pair<int, int>> my_localization(const Mat& img_gray)
{
	vector<pair<int, int>> results;
	for (int i = 0; i < img_gray.rows; ++i)
	{
		// i is row
		auto row_block = my_row_count(img_gray, i); // 返回行块 (len, scalar)
		auto cols = my_row_check_ratio(row_block); // 返回列
		for (auto col : cols) // 横向检查，如果非空，说明找到了横向的11311
		{
			// 找到横向特征，再检查垂直方向
			auto col_block = my_col_count(img_gray, col);
			auto rows = my_col_check_ratio(col_block);
			for (auto row : rows) // 如果垂直方向非空, 也符合11311
			{
				// 再做一次横向检查，交点即为中心
				auto row_block = my_row_count(img_gray, row);
				auto cols = my_row_check_ratio(row_block);
				for (auto col : cols)
				{
					results.push_back({ row, col });
					// cout << "Center: " << row << ", " << col << endl;
				}
			}
		}
	}
	results = my_make_point_unique(results);
	return results;
}

Mat my_rgb2gray(const Mat& img_rgb)
{
	int row = img_rgb.rows;
	int col = img_rgb.cols;
	Mat img_gray(row, col, CV_8U, Scalar(0));
	for (size_t i = 0; i < row; ++i)
	{
		for (size_t j = 0; j < col; ++j)
		{
			Vec3b rgb = img_rgb.at<Vec3b>(i, j);
			img_gray.at<uchar>(i, j) = 0.299 * rgb[0] + 0.587 * rgb[1] + 0.114 * rgb[2];
		}
	}
	return img_gray;
}

Mat my_medfilter(const Mat& img_gray)
{
	const int len = 9;
	const int len_half = len / 2;
	uchar window[len];
	int rows = img_gray.rows;
	int cols = img_gray.cols;
	Mat results;
	img_gray.copyTo(results);
	for (size_t i = 0; i < rows; ++i)
	{
		for (size_t j = 0; j < cols; ++j)
		{
			std::fill(std::begin(window), std::end(window), 0);
			int w_idx = 0;
			for (int x = -1; x <= 1; ++x)
			{
				for (int y = -1; y <= 1; ++y)
				{
					if (i + x >= 0 && i + x < rows && j + y >= 0 && j + y < cols)
					{
						window[w_idx++] = results.at<uchar>(i + x, j + y);
					}
				}
			}
			int k = 0;
			while (w_idx < 9)
			{
				window[w_idx++] = window[k];
			}
			std::nth_element(std::begin(window), std::begin(window) + len_half, std::end(window));
			results.at<uchar>(i, j) = window[len_half];
		}
	}
	return results;
}

Mat my_im2bw(const Mat& img_gray)
{
	int min = img_gray.at<uchar>(0, 0);
	int max = min;
	Mat results;
	img_gray.copyTo(results);
	for (size_t i = 0; i < results.rows; ++i)
	{
		for (size_t j = 0; j < results.cols; ++j)
		{
			int x = static_cast<int>(results.at<uchar>(i, j));
			min = std::min(min, x);
			max = std::max(max, x);
		}
	}
	int th = (min + max) / 2;
	for (int i = 0; i < results.rows; ++i)
	{
		for (int j = 0; j < results.cols; ++j)
		{
			results.at<uchar>(i, j) = results.at<uchar>(i, j) > th ? 255 : 0;
		}
	}
	return results;
}

void my_draw_line_row(Mat& img, int row)
{
	//cout << "-" << endl;;
	for (int j = 0; j < img.cols; ++j)
	{
		img.at<uchar>(row, j) = 0;
	}
}

void my_draw_line_col(Mat& img, int col)
{
	//cout << "|" << endl;;
	for (int j = 0; j < img.rows; ++j)
	{
		img.at<uchar>(j, col) = 0;
	}
}

Mat my_rotate(cv::Mat img, double angle)
{
	int len = static_cast<int>(sqrtf(pow(img.rows, 2) + pow(img.cols, 2)) + 0.5);

	Mat result = Mat::zeros(len, len, CV_8U);
	double rad = angle * PI / 180;
	int xSm = 0;
	int ySm = 0;
	for (int i = 0; i < result.rows; i++)
	{
		for (int j = 0; j < result.cols; j++)
		{
			xSm = static_cast<int>((i - result.rows / 2)*cos(rad) - (j - result.cols / 2)*sin(rad) + 0.5);
			ySm = static_cast<int>((i - result.rows / 2)*sin(rad) + (j - result.cols / 2)*cos(rad) + 0.5);
			xSm += img.rows / 2;
			ySm += img.cols / 2;
			if (xSm >= img.rows || ySm >= img.cols || xSm <= 0 || ySm <= 0) 
			{
				result.at<uchar>(i, j) = 0;
			}
			else 
			{
				result.at<uchar>(i, j) = img.at<uchar>(xSm, ySm);
			}
		}
	}
	return result;
}

Mat my_qr_rectified(const Mat& img, vector<My_Point> points)
{
	My_Point A = points[0];
	My_Point B = points[1];
	My_Point C = points[2];
	double AB = my_distance(A, B);
	double AC = my_distance(A, C);
	double BC = my_distance(B, C);
	double r = 0;
	My_Point left_top;
	My_Point O;
	if (AB > AC && AB > BC)
	{
		left_top = C;
		O = make_pair((A.first + B.first) / 2, (A.second + B.second) / 2);
		r = max(std::abs(A.first - O.first), std::abs(A.second - O.second));

	}
	else if (AC > AB && AC > BC)
	{
		left_top = B;
		O = make_pair((A.first + C.first) / 2, (A.second + C.second) / 2);
		r = max(std::abs(A.first - O.first), std::abs(A.second - O.second));
	}
	else if (BC > AB && BC > AC)
	{
		left_top = A;
		O = make_pair((B.first + C.first) / 2, (B.second + C.second) / 2);
		r = max(std::abs(B.first - O.first), std::abs(B.second - O.second));
	}

	My_Point left_top_O = make_pair(O.first - static_cast<int>(r), O.second - static_cast<int>(r));
	cout << "O: " << "(" << O.first << ", " << O.second << ")" << endl;
	cout << "left_top_O: " << "(" << left_top_O.first << ", " << left_top_O.second << ")" << endl;

	double err = 0;
	// line input:x output:y
	auto line = [O, left_top_O](double x) -> double
	{
		return (O.first - left_top_O.first)*(x - left_top_O.first) / (O.second - left_top_O.second) + left_top_O.second;
		// y = (y1-y0)/(x1-x0)*(x-x0)+y0
	};
	if (O.second != left_top_O.second)
	{
		if (left_top.second > line(left_top.first))
		{
			left_top.first = 2 * O.first - left_top.first;
			left_top.second = 2 * O.second - left_top.second;
			err = 180;
		}
	}
	cout << "err is: " << err << endl;
	double o_lto = my_distance(O, left_top_O);
	double o_lt = my_distance(O, left_top);
	double lto_lt = my_distance(left_top_O, left_top);
	
	double theta = 0;
	if (o_lto != 0 && o_lt != 0)
	{
		double cos_theta = (o_lto*o_lto + o_lt*o_lt - lto_lt*lto_lt) / (2 * o_lto*o_lt);
		theta = acos(cos_theta);
	}
	cout << "theta: " << theta / PI << "π" << endl;
	double angle = theta / PI*static_cast<double>(180);
	return my_rotate(img, angle + err);
}

int my_qr_px_size(const Mat& img)
{
	vector<My_Point> points = my_localization(img);
	vector<int> rows;
	vector<int> cols;
	My_Point left_top(0, 0);
	int p_count = 0;
	int width = 0;
	int px = 0;
	for (auto p : points)
	{
		rows.push_back(p.first);
		cols.push_back(p.second);
		cout << ": (" << p.first << ", " << p.second << ")" << endl;
	}
	for (auto row : rows)
	{
		auto row_block = my_row_count(img, row);
		size_t len = row_block.size();
		vector<int> temp;
		vector<int> rows;
		for (size_t i = 0; i < len; ++i)
		{
			temp.clear();
			if (len - i >= 5 && row_block[i].second == 0) // first block is black
			{
				for (int j = i; j < i + 5; ++j)
				{
					int x = static_cast<int>(static_cast<double>(row_block[j].first) / static_cast<double>(row_block[i].first) + 0.5);
					temp.push_back(x);
				}
				if (temp[0] == 1 && temp[1] == 1 && temp[2] == 3 && temp[3] == 1 && temp[4] == 1)
				{
					auto& rb = row_block;
					px = (px + (rb[i].first + rb[i + 1].first + rb[i + 2].first + rb[i + 3].first + rb[i + 4].first) / 7) / 2;
				}
			}
		}
	}
	cout << "average px: " << px << endl;
	return px;
}

Mat my_qr_cut(const Mat& img, int pw)
{
	vector<My_Point> points = my_localization(img);
	My_Point A = points[0];
	My_Point B = points[1];
	My_Point C = points[2];
	double AB = my_distance(A, B);
	double AC = my_distance(A, C);
	double BC = my_distance(B, C);
	int width = 0;
	My_Point left_top;
	My_Point left_buttom;
	int r = 3.8 * pw + 0.5;
	if (AB > AC && AB > BC)
	{
		left_top = make_pair(C.first - r + 0.5, C.second - r + 0.5);
		width = static_cast<int>(AC) + 2 * r + 0.5;
	}
	else if (AC > AB && AC > BC)
	{
		left_top = make_pair(B.first - r + 0.5, B.second - r + 0.5);
		width = static_cast<int>(AB) + 2 * r + 0.5;
	}
	else if (BC > AB && BC > AC)
	{
		left_top = make_pair(A.first - r + 0.5, A.second - r + 0.5);
		width = static_cast<int>(AB) + 2 * r + 0.5;
	}
	Mat result(width, width, CV_8U, Scalar(0));
	for (size_t i = 0; i < width; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			result.at<uchar>(i, j) = img.at<uchar>(i + left_top.first, j + left_top.second);
		}
	}
	return result;
}

Mat my_qr_sampling(const Mat& img, int pw)
{
	const int version_1 = 21;
	cout << "img width: " << img.rows << endl;
	int width = img.rows;
	int step = width / version_1 + 0.5;
	cout << "step: " << step << endl;
	cout << "width: " << width << endl;
	int white = 0;
	int black = 0;

	Mat temp;
	img.copyTo(temp);
	//namedWindow("test", WINDOW_NORMAL);

	Mat result(version_1, version_1, CV_8U, Scalar(0));
	for (size_t i = 0; i < width; i+=step)
	{
		for (size_t j = 0; j < width; j += step)
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

Mat my_resize(const Mat& src, int width, int heigh);

int main()
{
	string w_0("Window");
	namedWindow(w_0, WINDOW_NORMAL);

	Mat m = imread("qr_code.bmp");
	Mat m_gray = my_rgb2gray(m);
	Mat m_gray_medfilt = my_medfilter(m_gray);
	Mat m_gray_medfilt_b = my_im2bw(m_gray_medfilt);
	Mat mgmb; m_gray_medfilt_b.copyTo(mgmb);
	auto points = my_localization(m_gray_medfilt_b);
	Mat m_rec = my_qr_rectified(m_gray_medfilt_b, points);
	int pixel = my_qr_px_size(m_rec);
	Mat m_cut = my_qr_cut(m_rec, pixel);
	Mat m_cut_resize(168, 168, CV_8U, Scalar(0));
	resize(m_cut, m_cut_resize, cv::Size(168, 168));
	Mat finals = my_qr_sampling(m_cut_resize, pixel);
	imwrite("output\\origin.bmp", m);
	imwrite("output\\gray.bmp", m_gray);
	imwrite("output\\medfilt.bmp", m_gray_medfilt);
	imwrite("output\\binaryzation.bmp", m_gray_medfilt_b);
	imwrite("output\\rotation.bmp", m_rec);
	imwrite("output\\cut.bmp", m_cut);
	imwrite("output\\cut_resize.bmp", m_cut_resize);
	imwrite("output\\finals.bmp", finals);
	vector<Mat> one;
	one.push_back(m_gray);
	one.push_back(m_gray_medfilt);
	one.push_back(mgmb);
	one.push_back(m_gray_medfilt_b);
	one.push_back(m_rec);
	one.push_back(m_cut);
	one.push_back(finals);
	Mat x = createOne(one, 7, 10);


	imshow(w_0, x);
	waitKey();
	return 0;
}
