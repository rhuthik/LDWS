#include <iostream>
#include <opencv2/opencv.hpp>
#include <edgedetection/edge_sobel.hpp>


using namespace cv;
using namespace std;


cv::Mat edge_sobel::process(cv::Mat& input)
{
  cv::Mat output;
  cv::Sobel(input, output, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
  return output;
}
