#ifndef INCLUDE_DEBLUR_MEDIAN_HPP
#define INCLUDE_DEBLUR_MEDIAN_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
//using namespace std;


class deblur_median
{
public:

   void deblur_median_process(cv::Mat gray,cv::Mat& blur);

};

#endif
