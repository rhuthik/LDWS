#ifndef INCLUDE_BLUR_GAUSSIAN_HPP
#define INCLUDE_BLUR_GAUSSIAN_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class blur_gaussian
{
public:

   void process(cv::Mat input, cv::Mat output);

};

#endif
