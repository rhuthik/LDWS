#include <iostream>
#include <opencv2/opencv.hpp>
#include <thresholding/thresholding_maxentropy.hpp>


using namespace cv;
using namespace std;


void threshold_maxentropy::process(cv::Mat input, cv::Mat output)
{
  cv::threshold(input, output, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}