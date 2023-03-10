#ifndef INCLUDE_THRESHOLD_HPP
#define INCLUDE_THRESHOLD_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class threshold_image
{

public:

    bool is_variant(int threshold_selection);
    std::chrono::microseconds get_tick_count(int threshold_selection);
    void process(cv::Mat input, cv::Mat& output, std::chrono::microseconds* tick_count, int threshold_selection);
    
};


#endif