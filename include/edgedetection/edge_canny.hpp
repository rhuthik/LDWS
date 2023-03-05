#ifndef INCLUDE_EDGE_CANNY_HPP
#define INCLUDE_EDGE_CANNY_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class edge_canny
{
public:

  void process(cv::Mat input, cv::Mat output);

};

#endif
