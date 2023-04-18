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
  int threshold = 80;
    int max_gap = 10;

    vector<Vec4i> detected_lines;
    while (threshold > 10)
    {
        detected_lines.clear();
        for (size_t i = 0; i < lines.size(); i++)
        {
            float rho = lines[i][0], theta = lines[i][1];
            if (rho < 0)
            {
                rho = -rho;
                theta = theta - CV_PI;
            }
            if (rho < threshold)
            {
                Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a * rho, y0 = b * rho;
                pt1.x = cvRound(x0 + max_gap * (-b));
                pt1.y = cvRound(y0 + max_gap * (a));
                pt2.x = cvRound(x0 - max_gap * (-b));
                pt2.y = cvRound(y0 - max_gap * (a));
                detected_lines.push_back(Vec4i(pt1.x, pt1.y, pt2.x, pt2.y));
            }
        }
        threshold -= 5;
        max_gap += 2;
        if (!detected_lines.empty()) break;
    }


    //detected_lines have the coordinates
    //printing in image
    for (size_t i = 0; i < detected_lines.size(); i++)
    {
        float rho = detected_lines[i][0], theta = detected_lines[i][1];
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
  cv::imwrite("adap_hough.png", result);
  cv::waitKey();

  return 0;
}