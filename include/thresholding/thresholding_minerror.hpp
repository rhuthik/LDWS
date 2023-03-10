#ifndef INCLUDE_THRESHOLDING_MINERROR_HPP
#define INCLUDE_THRESHOLDING_MINERROR_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class threshold_minerror
{
public:

  void process(cv::Mat input, cv::Mat output);

};

#endif
