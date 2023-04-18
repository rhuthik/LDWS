#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <cmath>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    string fileone,filetwo;
    Mat img1, img2; 
    for(int m=0;m<14;m++){
    // Load the two images
        if(m<9){
            fileone= "f0000" + to_string(m) + ".png";
            filetwo= "f0000" + to_string(m+1) + ".png";
            img1 = imread(fileone, IMREAD_GRAYSCALE);
            img2 = imread(filetwo, IMREAD_GRAYSCALE);
        }
        else if(m==9){
            fileone= "f0000" + to_string(m) + ".png";
            filetwo= "f000" + to_string(m+1) + ".png";
            img1 = imread(fileone, IMREAD_GRAYSCALE);
            img2 = imread(filetwo, IMREAD_GRAYSCALE);
        }
        else{
            fileone= "f000" + to_string(m) + ".png";
            filetwo= "f000" + to_string(m+1) + ".png";
            img1 = imread(fileone, IMREAD_GRAYSCALE);
            img2 = imread(filetwo, IMREAD_GRAYSCALE);
        }
        

        // Calculate the MSE between the two images
        int N = img1.rows * img1.cols;
        double mse = 0;
        for (int i = 0; i < img1.rows; i++) {
            for (int j = 0; j < img1.cols; j++) {
                double diff = img1.at<uchar>(i,j) - img2.at<uchar>(i,j);
                mse += diff * diff;
            }
        }
        mse /= N;

        // Calculate the PSNR between the two images
        double psnr = 10 * log10(255 * 255 / mse);

        // Calculate the percentage of accuracy
        double max_psnr = 20 * log10(255);
        double accuracy = (psnr / max_psnr) * 100;

        // Print the results
        cout << "MSE: " << mse << endl;
        cout << "PSNR: " << psnr << endl;
        cout << "Accuracy of "<<m<<" & "<<(m+1)<<": " << accuracy << "%" << endl;
    }

    return 0;
}

// g++ accuracy.cpp -o temp `pkg-config --cflags --libs opencv4`