#include <iostream>
#include <opencv2/opencv.hpp>
#include <blurring/blur_histogram.hpp>


using namespace cv;
using namespace std;


void blur_histogram::process(cv::Mat input, cv::Mat output)
{
  // Calculate the histogram of the image
  cv::Mat hist;
  int channels[] = {0};
  int histSize[] = {256};
  float range[] = {0, 256};
  const float* ranges[] = {range};
  cv::calcHist(&input, 1, channels, Mat(), hist, 1, histSize, ranges);

    // Normalize the histogram
  cv::normalize(hist, hist, 0, input.rows, NORM_MINMAX, -1, Mat());

    // Create a lookup table for histogram equalization
  cv::Mat lut(1, 256, CV_8U);
  float alpha = 255.0 / (input.rows * input.cols);
  int sum = 0;
  for (int i = 0; i < 256; i++) 
  {
    sum += hist.at<float>(i);
    int val = cvRound(sum * alpha);
    lut.at<uchar>(i) = saturate_cast<uchar>(val);
  }

  // Apply the lookup table to the image
  cv::LUT(input, lut, output);
}
