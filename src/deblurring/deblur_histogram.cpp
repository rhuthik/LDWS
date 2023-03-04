#include <iostream>
#include <opencv2/opencv.hpp>
#include <deblurring/deblur_histogram.hpp>


using namespace cv;
//using namespace std;


void deblur_histogram::deblur_histogram_process(cv::Mat gray,cv::Mat& blur)
{
   GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);
   std::cout << "histogram\n";

}
