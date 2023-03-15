#include <iostream>
#include <opencv2/opencv.hpp>
#include <hough/hough_prob.hpp>


using namespace cv;
using namespace std;


void hough_prob::process(cv::Mat input, cv::Mat output)
{
    output=input.clone();
    std::vector<cv::Vec4i> linesp;
    HoughLinesP(input, linesp, 1, CV_PI/180, 50, 50, 10);
    // The coordinates are saved in the linesp vector
    // Draw the lines on the image
    for (size_t i = 0; i < linesp.size(); i++) {
        cv::line(output, cv::Point(linesp[i][0], linesp[i][1]),
            cv::Point(linesp[i][2], linesp[i][3]), cv::Scalar(0, 0, 255), 3,LINE_AA);
    }
}