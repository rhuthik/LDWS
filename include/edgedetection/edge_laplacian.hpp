#ifndef INCLUDE_EDGE_LAPLACIAN_HPP
#define INCLUDE_EDGE_LAPLACIAN_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class edge_laplacian
{
public:

  void process(cv::Mat input, cv::Mat output);

};

#endif
