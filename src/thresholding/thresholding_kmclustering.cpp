#include <iostream>
#include <opencv2/opencv.hpp>
#include <thresholding/thresholding_kmclustering.hpp>


using namespace cv;
using namespace std;


cv::Mat threshold_kmclustering::process(cv::Mat& input)
{
  cv::Mat output;
  int numClusters = 2; // number of clusters
  cv::Mat samples(input.rows * input.cols, 1, CV_32F);
  int sampleIdx = 0;
  for (int i = 0; i < input.rows; i++)
  {
    for (int j = 0; j < input.cols; j++) 
      {
        samples.at<float>(sampleIdx++) = input.at<uchar>(i, j);
      }
  }
  cv::Mat labels, centers;
  cv::kmeans(samples, numClusters, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);
  int threshold = (int)centers.at<float>(0, 0); // use the minimum cluster center as threshold
  if (centers.at<float>(1, 0) < threshold) threshold = (int)centers.at<float>(1, 0); // update the threshold if there is a lower center
  cv::threshold(input, output, threshold, 255, cv::THRESH_BINARY);
  return output;
}
