#include <iostream>
#include <opencv2/opencv.hpp>
#include <thresholding/thresholding_maxentropy.hpp>


using namespace cv;
using namespace std;


cv::Mat threshold_maxentropy::process(cv::Mat& input)
{
  cv::Mat output;
  int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };

    cv::Mat hist;
    cv::calcHist(&input, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    double sum = 0;
    for (int i = 0; i < histSize; i++)
    {
        sum += hist.at<float>(i, 0);
    }

    double maxEntropy = 0;
    int threshold = 0;

    for (int t = 0; t < histSize; t++)
    {
        double w1 = 0;
        double w2 = 0;
        double sum1 = 0;
        double sum2 = 0;

        for (int i = 0; i <= t; i++)
        {
            w1 += hist.at<float>(i, 0) / sum;
            sum1 += i * hist.at<float>(i, 0);
        }

        for (int i = t + 1; i < histSize; i++)
        {
            w2 += hist.at<float>(i, 0) / sum;
            sum2 += i * hist.at<float>(i, 0);
        }

        double entropy = -w1 * log2(w1) - w2 * log2(w2);

        if (entropy > maxEntropy)
        {
            maxEntropy = entropy;
            threshold = t;
        }
    }
    cv::threshold(input, output, threshold, 255, cv::THRESH_BINARY);
  return output;
}
