#ifndef INCLUDE_BLUR_BOX_HPP
#define INCLUDE_BLUR_BOX_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class blur_box
{
public:

  cv::Mat process(cv::Mat& input);

};

#endif
