#ifndef INCLUDE_EDGE_LAPLACIAN_HPP
#define INCLUDE_EDGE_LAPLACIAN_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class edge_laplacian
{
public:

  cv::Mat process(cv::Mat& input);

};

#endif
