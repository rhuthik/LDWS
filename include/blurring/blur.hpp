#ifndef INCLUDE_BLUR_HPP
#define INCLUDE_BLUR_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class blur_image
{

public:

    bool is_variant(int blur_selection);
    std::chrono::microseconds get_tick_count(int blur_selection);
    void process(cv::Mat input, cv::Mat& output, std::chrono::microseconds* tick_count, int blur_selection);
    
};


#endif