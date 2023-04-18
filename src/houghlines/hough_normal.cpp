#include <iostream>
#include <opencv2/opencv.hpp>
#include <houghlines/hough_normal.hpp>


using namespace cv;
using namespace std;


cv::Mat hough_normal::process(cv::Mat& input,cv::Mat input_image)
{
    cv::Mat output=input_image.clone();
    std::vector<Vec2f> lines;
    // The coordinates are saved in the linesp vector
    // Draw the lines on the image
    HoughLines(input, lines, 1, CV_PI/180, 150, 0, 0 );
  for (size_t i = 0; i < lines.size(); i++)
      {
          float rho = lines[i][0], theta = lines[i][1];
          Point pt1, pt2;
          double a = cos(theta), b = sin(theta);
          double x0 = a*rho, y0 = b*rho;
          pt1.x = cvRound(x0 + 1000*(-b));
          pt1.y = cvRound(y0 + 1000*(a));
          pt2.x = cvRound(x0 - 1000*(-b));
          pt2.y = cvRound(y0 - 1000*(a));
          line(output, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
      }
      return output;
}