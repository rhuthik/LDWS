#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
using namespace cv;
using namespace std;


int main() {
  cv::Mat image = cv::imread("road.jpg");
  // Check if image is empty
  if (image.empty()) {
    std::cout << "Could not read image" << std::endl;
    return 1;
  }
  // Convert the image to grayscale
  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
  // Apply Gaussian blur to reduce noise
  cv::Mat blur;
  GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);
  // Apply Canny edge detection
  cv::Mat edges;
  Canny(blur, edges, 50, 150);


// hough starts here
  std::vector<Vec2f> lines;//this holds the coordinates
  cv::Mat result = image.clone();
  HoughLines(edges, lines, 1, CV_PI/180, 150, 0, 0 );
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
          line(result, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
      }
// Show the result
  cv::imwrite("normalhough.png", result);
  cv::waitKey();

  return 0;
}