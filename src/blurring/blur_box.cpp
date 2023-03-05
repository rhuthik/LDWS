#include <iostream>
#include <opencv2/opencv.hpp>
#include <blurring/blur_box.hpp>


using namespace cv;
using namespace std;


void blur_box::process(cv::Mat input, cv::Mat output)
{
   cv::boxFilter(input, output, -1, cv::Size(5, 5));
}
