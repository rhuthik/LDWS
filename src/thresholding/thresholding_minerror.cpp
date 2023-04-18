#include <iostream>
#include <opencv2/opencv.hpp>
#include <thresholding/thresholding_minerror.hpp>


using namespace cv;
using namespace std;


cv::Mat threshold_minerror::process(cv::Mat& input)
{
  cv::Mat output;
  double maxValue = 255;
  int numIterations = 10;
  int threshold = cv::mean(input).val[0]; // initial threshold
  for (int i = 0; i < numIterations; i++)
  {
    cv::Mat mask;
    cv::threshold(input, mask, threshold, maxValue, cv::THRESH_BINARY);
    double mean1 = cv::mean(input, mask).val[0];
    double mean2 = cv::mean(input, ~mask).val[0];
    threshold = (mean1 + mean2) / 2;
  }
  cv::threshold(input, output, threshold, 255, cv::THRESH_BINARY);
  return output;
}
