#ifndef INCLUDE_DEBLUR_BOX_HPP
#define INCLUDE_DEBLUR_BOX_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
//using namespace std;


class deblur_box
{
public:

   void deblur_box_process(cv::Mat gray,cv::Mat& blur);

};

#endif
