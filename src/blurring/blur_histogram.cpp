#include <iostream>
#include <opencv2/opencv.hpp>
#include <blurring/blur_histogram.hpp>


using namespace cv;
using namespace std;


cv::Mat blur_histogram::process(cv::Mat& input)
{
  cv::Mat output;
  // Calculate the histogram of the image
  cv::equalizeHist(input,output);
  cv::GaussianBlur(output, output, cv::Size(3, 3), 0, 0);
  return output;
}
