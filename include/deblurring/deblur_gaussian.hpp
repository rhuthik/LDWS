#ifndef INCLUDE_DEBLUR_GAUSSIAN_HPP
#define INCLUDE_DEBLUR_GAUSSIAN_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
//using namespace std;


class deblur_gaussian
{
public:

   void deblur_gaussian_process(cv::Mat gray,cv::Mat& blur);

};

#endif
