#include <iostream>
#include <opencv2/opencv.hpp>
#include <edgedetection/edge_laplacian.hpp>


using namespace cv;
using namespace std;


void edge_laplacian::process(cv::Mat input, cv::Mat output)
{
   cv::Laplacian(input, output, CV_8U, 3, 1, 0, BORDER_DEFAULT);
}
