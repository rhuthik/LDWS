#ifndef INCLUDE_HOUGH_HPP
#define INCLUDE_HOUGH_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class hough_image
{

public:

    bool is_variant(int hough_selection);
    std::chrono::microseconds get_tick_count(int hough_selection);
    void process(cv::Mat input, cv::Mat& output, std::chrono::microseconds* tick_count, int hough_selection);
    
};


#endif