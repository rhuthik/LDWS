#ifndef INCLUDE_HOUGH_ADAPTIVE_HPP
#define INCLUDE_HOUGH_ADAPTIVE_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class hough_adaptive
{
public:

  cv::Mat process(cv::Mat& input, cv::Mat input_image);

};

#endif
