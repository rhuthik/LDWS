#ifndef INCLUDE_BLUR_HISTOGRAM_HPP
#define INCLUDE_BLUR_HISTOGRAM_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class blur_histogram
{
public:

   void process(cv::Mat input, cv::Mat output);

};

#endif
