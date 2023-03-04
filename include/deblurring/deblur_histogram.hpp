#ifndef INCLUDE_DEBLUR_HISTOGRAM_HPP
#define INCLUDE_DEBLUR_HISTOGRAM_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
//using namespace std;


class deblur_histogram
{
public:

   void deblur_histogram_process(cv::Mat gray,cv::Mat& blur);

};

#endif
