#include <iostream>
#include <opencv2/opencv.hpp>
#include <blurring/blur_gaussian.hpp>


using namespace cv;
using namespace std;


void blur_gaussian::process(cv::Mat input, cv::Mat output)
{
  cv::GaussianBlur(input, output, cv::Size(5, 5), 0, 0);

}
