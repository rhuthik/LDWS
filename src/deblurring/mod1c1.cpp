#include <iostream>
#include <deblurring/mod1c1.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
void mod1c1::foo(Mat& gray,Mat& blur)
{
   cv::GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);
   std::cout << "mod1c1\n";

}
