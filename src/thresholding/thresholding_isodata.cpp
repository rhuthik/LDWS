#include <iostream>
#include <opencv2/opencv.hpp>
#include <thresholding/thresholding_isodata.hpp>


using namespace cv;
using namespace std;


cv::Mat threshold_isodata::process(cv::Mat& input)
{
  cv::Mat output;
  double maxValue = 255;
  double threshold = cv::mean(input).val[0]; // initial threshold
  double delta = 1.0;
  while (delta > 0.5) 
  {
    cv::Mat mask;
    cv::threshold(input, mask, threshold, maxValue, cv::THRESH_BINARY);
    double mean1 = cv::mean(input, mask).val[0];
    double mean2 = cv::mean(input, ~mask).val[0];
    double newThreshold = (mean1 + mean2) / 2;
    delta = std::abs(newThreshold - threshold);
    threshold = newThreshold;
  }
  cv::threshold(input, output, threshold, 255, cv::THRESH_BINARY);
  return output;
}
