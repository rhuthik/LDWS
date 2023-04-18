#ifndef INCLUDE_HOUGH_PROB_HPP
#define INCLUDE_HOUGH_PROB_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class hough_prob
{
public:

  cv::Mat process(cv::Mat& input,cv::Mat input_image);

};

#endif
