#ifndef INCLUDE_THRESHOLDING_MAXENTROPY_HPP
#define INCLUDE_THRESHOLDING_MAXENTROPY_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class threshold_maxentropy
{
public:

  cv::Mat process(cv::Mat& input);

};

#endif
