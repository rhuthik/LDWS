#ifndef INCLUDE_THRESHOLDING_OTSU_HPP
#define INCLUDE_THRESHOLDING_OTSU_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class threshold_otsu
{
public:

  cv::Mat process(cv::Mat& input);

};

#endif
