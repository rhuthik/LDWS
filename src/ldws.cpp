#include <deblurring/mod1c1.hpp>
#include <deblurring/mod1c2.hpp>
#include <module2/mod2c1.hpp>
#include <module2/mod2c2.hpp>
#include <shedutil/schedule.hpp>
#include <ldws.hpp>
#include <constants.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
schedule scheduler;

int main()
{
   cv::Mat image = cv::imread("road.jpg");
   if (image.empty()) {
    std::cout << "Could not read image" << std::endl;
    return 1;
   }
   //   Convert to grayscale
   cv::Mat gray;
   cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
   mod1c1 m1c1;
   mod1c2 m1c2;
   mod2c1 m2c1;
   mod2c2 m2c2;
   // Blurring module
   cv::Mat blur;
   m1c1.foo(gray,blur);
   m1c2.foo();
   m2c1.foo();
   m2c2.foo();

   program::foo();

   scheduler.initialize();
   // Show the result
   cv::imwrite("output.png", blur);
   cv::waitKey();

   return 0;
}
