#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main() {
    cv::Mat frame = cv::imread("road.jpg");

    if (frame.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    // Define the region of interest (ROI) for lane detection
    cv::Rect roi(0, frame.rows * 0.5, frame.cols, frame.rows * 0.5);

    // Apply ROI to the current frame
    cv::Mat roi_frame = frame(roi);

    // Convert the ROI frame to grayscale
    cv::Mat gray;
    cv::cvtColor(roi_frame, gray, cv::COLOR_BGR2GRAY);

    // Apply Otsu's thresholding to the grayscale image
    cv::Mat binary;
    cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Apply Canny edge detection to the thresholded image
    cv::Mat edges;
    cv::Canny(binary, edges, 50, 150);

    // Apply Hough transform to the Canny edge image to detect lines
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(edges, lines, 1, CV_PI / 180, 50, 50, 10);

    // Draw the detected lines on the original image
    for (size_t i = 0; i < lines.size(); i++) {
        cv::line(frame, cv::Point(lines[i][0], lines[i][1] + roi.y),
                 cv::Point(lines[i][2], lines[i][3] + roi.y), cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
    }

    // Display the original image with detected lane lines
    cv::imwrite("otsu.png", frame);
    cv::waitKey(0);

    return 0;
}
