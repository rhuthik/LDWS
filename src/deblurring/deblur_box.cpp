#include <iostream>
#include <opencv2/opencv.hpp>
#include <deblurring/deblur_box.hpp>


using namespace cv;
//using namespace std;


void deblur_box::deblur_box_process(cv::Mat gray,cv::Mat& blur)
{
   GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);
   std::cout << "box\n";

}
