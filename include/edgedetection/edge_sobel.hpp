#ifndef INCLUDE_EDGE_SOBEL_HPP
#define INCLUDE_EDGE_SOBEL_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class edge_sobel
{
public:

  cv::Mat process(cv::Mat& input);

};

#endif
