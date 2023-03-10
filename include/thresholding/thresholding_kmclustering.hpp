#ifndef INCLUDE_THRESHOLDING_KMCLUSTERING_HPP
#define INCLUDE_THRESHOLDING_KMCLUSTERING_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class threshold_kmclustering
{
public:

  void process(cv::Mat input, cv::Mat output);

};

#endif
