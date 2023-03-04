// #define CV_VERSION_EPOCH 3
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
cv::Mat img1 = cv::imread("road.jpg", cv::IMREAD_GRAYSCALE);
cv::Mat img2 = cv::imread("blur1.png", cv::IMREAD_GRAYSCALE);

double ssimValue = 0.0;
cv::Scalar ssim = cv::compareSSIM(img1, img2, ssimValue,cv::noArray());

std::cout << "SSIM value: " << ssimValue << std::endl;
std::cout << "Percentage similarity: " << ssim[0] * 100 << "%" << std::endl;

cv::Mat diff;
cv::absdiff(img1, img2, diff);
double mse = cv::mean(diff.mul(diff))[0];
std::cout << "MSE: " << mse << std::endl;

// Calculate PSNR
double psnr = 0.0;
if (mse != 0.0) {
    psnr = 10.0 * log10((255.0 * 255.0) / mse);
}
std::cout << "PSNR: " << psnr << " dB" << std::endl;
return 0;
}