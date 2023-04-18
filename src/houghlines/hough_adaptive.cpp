#include <iostream>
#include <opencv2/opencv.hpp>
#include <houghlines/hough_adaptive.hpp>


using namespace cv;
using namespace std;


cv::Mat hough_adaptive::process(cv::Mat& input,cv::Mat input_image)
{
    cv::Mat output=input_image.clone();
    std::vector<Vec2f> lines;
    // The coordinates are saved in the linesp vector
    // Draw the lines on the image
    HoughLines(input, lines, 1, CV_PI/180, 150, 0, 0 );
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
        line(output, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
    }
    return output;
}