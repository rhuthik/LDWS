#ifndef INCLUDE_BLUR_MEDIAN_HPP
#define INCLUDE_BLUR_MEDIAN_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class blur_median
{
public:

   void process(cv::Mat input, cv::Mat output);

};

#endif
