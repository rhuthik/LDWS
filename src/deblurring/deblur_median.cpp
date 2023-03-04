#include <iostream>
#include <opencv2/opencv.hpp>
#include <deblurring/deblur_median.hpp>


using namespace cv;
//using namespace std;


void deblur_median::deblur_median_process(cv::Mat gray,cv::Mat& blur)
{
   GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);
   std::cout << "median\n";

}
