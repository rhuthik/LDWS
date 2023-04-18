#include <iostream>
#include <opencv2/opencv.hpp>
#include <blurring/blur_median.hpp>


using namespace cv;
using namespace std;


cv::Mat blur_median::process(cv::Mat& input)
{
  cv::Mat output;
  cv::medianBlur(input,output,3);
  return output;
}
