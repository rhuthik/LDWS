#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;
vector<double> cos_vec(180), sin_vec(180);
vector<double> theta(180);

Mat padd(Mat image, int k)
{
    int m = max(image.rows, image.cols);
    if (m % k == 0)
        m = m;
    else
        m = ((m / k) + 1) * k;

    Scalar color(0, 0, 0);
    Mat result(m, m, image.type(), color);

    int x_center = (m - image.cols) / 2;
    int y_center = (m - image.rows) / 2;

    image.copyTo(result(Rect(x_center, y_center, image.cols, image.rows)));

    return result;
}

void init() {
    for (int i = 0; i < 180; i++) {
        theta[i] = i * M_PI / 180.0;
        cos_vec[i] = cos(theta[i]);
        sin_vec[i] = sin(theta[i]);
    }
}

int closest_value(int val) {
    int k=8;
    return val - val % k;
}

vector<vector<vector<int>>> lht(int o) {
    vector<vector<vector<int>>> coord(o, vector<vector<int>>(o, vector<int>(180, -1)));
    for (int i = 0; i < o; i++) {
        for (int j = 0; j < o; j++) {
            for (int t = 0; t < 180; t++) {
                coord[i][j][t] = i * cos_vec[t] + j * sin_vec[t];
            }
        }
    }
    return coord;
}

vector<vector<vector<int>>> local_origins(int o) {
    int k=8;
    vector<vector<vector<int>>> coord(k + 1, vector<vector<int>>(k + 1, vector<int>(180, -1)));
    for (int i = 0; i <= k; i++) {
        for (int j = 0; j <= k; j++) {
            for (int t = 0; t < 180; t++) {
                coord[i][j][t] = i * o * cos_vec[t] + j * o * sin_vec[t];
            }
        }
    }
    return coord;
}

vector<vector<double>> add(vector<vector<int>> edge,int o) {
    vector<int> edge_pixels_i, edge_pixels_j;
    for (int i = 0; i < edge.size(); i++) {
        for (int j = 0; j < edge[i].size(); j++) {
            if (edge[i][j] == 255) {
                edge_pixels_i.push_back(i);
                edge_pixels_j.push_back(j);
            }
        }
    }

    int rho_range = round(sqrt(pow(edge.size(), 2) + pow(edge[0].size(), 2)));
    vector<vector<double>> accumulator(2 * rho_range, vector<double>(theta.size(), 0.0));
    int k=8;
    int w = edge.size() / k;

    vector<vector<vector<int>>> LHT = lht(o);
    vector<vector<vector<int>>> LO = local_origins(o);

    for (int p = 0; p < edge_pixels_i.size(); p++) {
        for (int t = 0; t < theta.size(); t++) {
            int lh = LHT[closest_value(edge_pixels_j[p])][closest_value(edge_pixels_i[p])][t];
            int lo = LO[closest_value(edge_pixels_j[p]) / w][closest_value(edge_pixels_i[p]) / w][t];
            int rho = lh + lo;
            accumulator[rho + rho_range][t] += 2.0;
        }
    }

    return accumulator;
}

int main()
{
    int k = 8; // replace with desired value
    Mat image = imread("road.jpg");
    Mat padded_image = padd(image, k);
    cv::Mat gray;
    cv::cvtColor(padded_image, gray, cv::COLOR_BGR2GRAY);

    // Apply Gaussian blur to reduce noise
    cv::Mat blur;
    cv::GaussianBlur(gray, blur, cv::Size(5, 5), 0, 0);

    // Apply Canny edge detection
    cv::Mat edges;
    cv::Canny(blur, edges, 50, 150);

    
    int m = padded_image.rows;
    int o=int(m/k);
    init();
    vector<vector<double>> accumulator = add(edges,o);
    Mat result(accumulator.size(), accumulator[0].size(), CV_8UC1);
    for (int i = 0; i < accumulator.size(); i++) {
        for (int j = 0; j < accumulator[0].size(); j++) {
            result.at<uchar>(i, j) = (uchar)accumulator[i][j];
        }
    }
    cv::imwrite("additive.png", result);
    cv::waitKey();

    return 0;
}