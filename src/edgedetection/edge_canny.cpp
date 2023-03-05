#include <iostream>
#include <opencv2/opencv.hpp>
#include <edgedetection/edge_canny.hpp>


using namespace cv;
using namespace std;


void edge_canny::process(cv::Mat input, cv::Mat output)
{
  cv::Canny(input, output, 50, 150);

}
