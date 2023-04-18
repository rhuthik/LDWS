// #define CV_VERSION_EPOCH 3
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <chrono>
using namespace cv;
using namespace std;


int main() {
cv::Mat img1 = cv::imread("road.jpg", cv::IMREAD_GRAYSCALE);
cv::Mat img2 = cv::imread("edges1.jpg", cv::IMREAD_GRAYSCALE);

// double ssimValue = 0.0;
// double ssim = ssim(img1, img2);

// std::cout << "SSIM value: " << ssim << std::endl;
// std::cout << "Percentage similarity: " << ssim * 100 << "%" << std::endl;
auto start_time = std::chrono::steady_clock::now();
cv::Mat diff;
cv::absdiff(img1, img2, diff);
double mse = cv::mean(diff.mul(diff))[0];
auto end_time = std::chrono::steady_clock::now();
auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
std::cout << "Elapsed time mse: "<<"  "  << elapsed_time.count() << " us" << std::endl;
std::cout << "MSE: " << mse << std::endl;

// Calculate PSNR
double psnr = 0.0;
if (mse != 0.0) {
    psnr = 10.0 * log10((255.0 * 255.0) / mse);
}
std::cout << "PSNR: " << psnr << " dB" << std::endl;
return 0;
}