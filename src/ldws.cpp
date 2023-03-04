#include <iostream>
#include <opencv2/opencv.hpp>

#include <ldws.hpp>
#include <constants.hpp>
#include <shedutil/schedule.hpp>


using namespace cv;
using namespace std;


schedule scheduler;

int main()
{
   //cv::Mat image = cv::imread("road.jpg");
   //if (image.empty()) {
   // std::cout << "Could not read image" << std::endl;
   // return 1;
   //}
   //   Convert to grayscale
   //cv::Mat gray;
   //cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
   // Blurring module
   //cv::Mat blur;

   scheduler.initialize();
   // Show the result
   //cv::imwrite("output.png", blur);
   //cv::waitKey();

   return 0;
}
