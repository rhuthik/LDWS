#ifndef INCLUDE_EDGE_HPP
#define INCLUDE_EDGE_HPP

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


class detect_edges
{

public:

    bool is_variant(int edge_detection_selection);
    std::chrono::microseconds get_tick_count(int edge_detection_selection);
    void process(cv::Mat input, cv::Mat& output, std::chrono::microseconds* tick_count, int edge_detection_selection);
    
};


#endif