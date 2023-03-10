#include <iostream>
#include <opencv2/opencv.hpp>
#include <thresholding/thresholding_adaptive.hpp>


using namespace cv;
using namespace std;


void threshold_adaptive::process(cv::Mat input, cv::Mat output)
{
  double maxValue = 255;
  int blockSize = 11; // block size for local region
  double C = 2.0; // constant to subtract from mean
  cv::adaptiveThreshold(input, output, maxValue, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, C);
}
