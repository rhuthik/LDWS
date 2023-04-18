#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
using namespace cv;
using namespace std;

void blurring(cv::Mat gray,cv::Mat& blur,int m_one){
  if(m_one==1)
    GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);
  else if(m_one==2){
    medianBlur(gray,blur,3);
  }
  else if(m_one==4){
    // Calculate the histogram of the image
    Mat hist;
    int channels[] = {0};
    int histSize[] = {256};
    float range[] = {0, 256};
    const float* ranges[] = {range};
    calcHist(&gray, 1, channels, Mat(), hist, 1, histSize, ranges);

    // Normalize the histogram
    normalize(hist, hist, 0, gray.rows, NORM_MINMAX, -1, Mat());

    // Create a lookup table for histogram equalization
    Mat lut(1, 256, CV_8U);
    float alpha = 255.0 / (gray.rows * gray.cols);
    int sum = 0;
    for (int i = 0; i < 256; i++) {
        sum += hist.at<float>(i);
        int val = cvRound(sum * alpha);
        lut.at<uchar>(i) = saturate_cast<uchar>(val);
    }

    // Apply the lookup table to the image
    LUT(gray, lut, blur);
  }
  else if(m_one==3){
    int kernel_size=3;
    boxFilter(gray, blur, -1, Size(kernel_size, kernel_size));
  }

}

void edge_detection(Mat blur,Mat& edges,int m_two){
  if(m_two==1){
    Canny(blur, edges, 50, 150);
  }
  else if(m_two==2){
    Sobel(blur,edges,CV_8U,1, 0, 3, 1, 0, BORDER_DEFAULT);
  }
  else if(m_two==3){
    Laplacian(blur, edges, CV_8U, 3, 1, 0, BORDER_DEFAULT);
  }
}

void Hough_Transforms(Mat result,Mat edges,vector<cv::Vec2f>& lines,int m_three){
  if(m_three==1){
   HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10); //proba
}
  if(m_three==2){
    HoughLines(edges, lines, 1, CV_PI/180, 150, 0, 0 ); //normal 
  }
  else if(m_three==3){
    HoughLines(edges, lines, 1, CV_PI / 180, 50, 0, 0); //adaptive
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
    // lines.clear();
    // copy(detected_lines.begin(), detected_lines.end(), std::back_inserter(lines));
  }
}


void getticks(){

  cv::Mat image = cv::imread("road.jpg");

  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

  // Apply Gaussian blur to reduce noise
  cv::Mat blur;
  for (int i=1;i<4;i++){
    auto start_time = std::chrono::steady_clock::now();

  // Call function to measure
  blurring(gray,blur,i);

  // Get end time
  auto end_time = std::chrono::steady_clock::now();

  // Calculate elapsed time in microsecs
  auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  // a[i]=elapsed_time;
  std::cout << "Elapsed time blur: "<<i<<"  " << elapsed_time.count() << " us" << std::endl;
  }
  
  blurring(gray,blur,1);
  cv::Mat edges;
  for (int i=1;i<4;i++){
    auto start_time = std::chrono::steady_clock::now();

  // Call function to measure
  edge_detection(blur,edges,i);

  // Get end time
  auto end_time = std::chrono::steady_clock::now();

  // Calculate elapsed time in microsecs
  auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  // a[i]=elapsed_time;
  std::cout << "Elapsed time edge: "<<i<<"  "  << elapsed_time.count() << " us" << std::endl;
  }
  edge_detection(blur,edges,1);

  std::vector<cv::Vec4i> linesp;
  auto start_time = std::chrono::steady_clock::now();
  HoughLinesP(edges, linesp, 1, CV_PI/180, 50, 50, 10);
  auto end_time = std::chrono::steady_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  std::cout << "Elapsed time hough: "<<"  "  << elapsed_time.count() << " us" << std::endl;


}


int main() {
  // initailising step to check ticks
  // getticks();
  auto start_time = std::chrono::steady_clock::now();
  // Read the road image
  cv::Mat image = cv::imread("/home/cps/ldws/dataset/inputs/test/f00249.png");

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
  blurring(gray,blur,1);
  // cv::GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);

  // Apply Canny edge detection
  cv::Mat edges;
  edge_detection(blur,edges,1);
  int h=1;
  cv::Mat result = image.clone();
  // Find lines using Hough transform
  if(h==1){
    std::vector<cv::Vec4i> linesp;
    HoughLinesP(edges, linesp, 1, CV_PI/180, 50, 50, 10);
    // Draw the lines on the image
    for (size_t i = 0; i < linesp.size(); i++) {
      cv::line(result, cv::Point(linesp[i][0], linesp[i][1]),
              cv::Point(linesp[i][2], linesp[i][3]), cv::Scalar(0, 0, 255), 3,LINE_AA);
  }
  }
  else if(h==2||h==3){
    std::vector<Vec2f> lines;
    Hough_Transforms(result,edges,lines,h);
    // Draw the lines on the image
    if(h==2){
      for (size_t i = 0; i < lines.size(); i++)
      {
          float rho = lines[i][0], theta = lines[i][1];
          Point pt1, pt2;
          double a = cos(theta), b = sin(theta);
          double x0 = a*rho, y0 = b*rho;
          pt1.x = cvRound(x0 + 1000*(-b));
          pt1.y = cvRound(y0 + 1000*(a));
          pt2.x = cvRound(x0 - 1000*(-b));
          pt2.y = cvRound(y0 - 1000*(a));
          line(result, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
      }
    }
  }
  // cv::HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10);

  auto end_time = std::chrono::steady_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
  std::cout << "Elapsed time total: "<<"  "  << elapsed_time.count() << " us" << std::endl;

  // Show the result
  cv::imwrite("/home/cps/ldws/dataset/references/f00249.png", result);
  cv::waitKey();

  return 0;
}