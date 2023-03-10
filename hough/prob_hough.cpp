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


  // Hough transform starts from here

  cv::Mat result = image.clone();
  std::vector<cv::Vec4i> linesp;
  HoughLinesP(edges, linesp, 1, CV_PI/180, 50, 50, 10);
  // The coordinates are saved in the linesp vector
  // Draw the lines on the image
  for (size_t i = 0; i < linesp.size(); i++) {
    cv::line(result, cv::Point(linesp[i][0], linesp[i][1]),
            cv::Point(linesp[i][2], linesp[i][3]), cv::Scalar(0, 0, 255), 3,LINE_AA);
  }
  // Show the result
  cv::imwrite("probhough.png", result);
  cv::waitKey();

  return 0;
}