#ifndef INCLUDE_DEBLUR_HPP
#define INCLUDE_DEBLUR_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class deblur_image
{

public:

    bool is_variant(int deblur_selection);
    //void get_tick_count(cv::Mat gray,cv::Mat& blur, int deblur_selection_id);
    //void process(cv::Mat gray,cv::Mat& blur, int deblur_selection_id);
    
};


#endif