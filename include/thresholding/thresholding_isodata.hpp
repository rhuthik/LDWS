#ifndef INCLUDE_THRESHOLDING_ISODATA_HPP
#define INCLUDE_THRESHOLDING_ISODATA_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class threshold_isodata
{
public:

  cv::Mat process(cv::Mat& input);

};

#endif
