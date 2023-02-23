#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
  // Read the road image
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
  cv::GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);

  // Apply Canny edge detection
  cv::Mat edges;
  cv::Canny(blur, edges, 50, 150);

  // Find lines using Hough transform
  std::vector<cv::Vec4i> lines;
  cv::HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10);

  // Draw the lines on the image
  cv::Mat result = image.clone();
  for (size_t i = 0; i < lines.size(); i++) {
    cv::line(result, cv::Point(lines[i][0], lines[i][1]),
             cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0, 0, 255), 3);
  }

  // Show the result
  cv::imwrite("blur.png", blur);
  cv::waitKey();

  return 0;
}