// Adaptive thresholding
cv::Mat src, dst;
double maxValue = 255;
int blockSize = 11; // block size for local region
double C = 2.0; // constant to subtract from mean
cv::adaptiveThreshold(src, dst, maxValue, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, C);

// Maximum entropy thresholding
cv::Mat src, dst;
cv::threshold(src, dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
cv::Mat hist;
cv::calcHist(&src, 1, 0, cv::Mat(), hist, 1, &maxValue, false);
hist /= src.total();
cv::Mat logP, H1, H2;
cv::log(hist, logP);
cv::integral(hist, H1, CV_64F);
cv::integral(logP, H2, CV_64F);
int threshold = 0;
double maxEntropy = -DBL_MAX;
for (int i = 0; i < maxValue; i++) {
    double p1 = H1.at<double>(i) / src.total();
    double p2 = 1 - p1;
    if (p1 == 0 || p2 == 0) continue;
    double logP1 = H2.at<double>(i) / H1.at<double>(i);
    double logP2 = (H2.at<double>(maxValue) - H2.at<double>(i)) / (src.total() - H1.at<double>(i));
    double entropy = -p1 * logP1 - p2 * logP2;
    if (entropy > maxEntropy) {
        maxEntropy = entropy;
        threshold = i;
    }
}
cv::threshold(src, dst, threshold, 255, cv::THRESH_BINARY);

// Minimum error thresholding
cv::Mat src, dst;
double maxValue = 255;
int numIterations = 10;
int threshold = cv::mean(src).val[0]; // initial threshold
for (int i = 0; i < numIterations; i++) {
    cv::Mat mask;
    cv::threshold(src, mask, threshold, maxValue, cv::THRESH_BINARY);
    double mean1 = cv::mean(src, mask).val[0];
    double mean2 = cv::mean(src, ~mask).val[0];
    threshold = (mean1 + mean2) / 2;
}
cv::threshold(src, dst, threshold, 255, cv::THRESH_BINARY);

// Isodata thresholding
cv::Mat src, dst;
double maxValue = 255;
double threshold = cv::mean(src).val[0]; // initial threshold
double delta = 1.0;
while (delta > 0.5) {
    cv::Mat mask;
    cv::threshold(src, mask, threshold, maxValue, cv::THRESH_BINARY);
    double mean1 = cv::mean(src, mask).val[0];
    double mean2 = cv::mean(src, ~mask).val[0];
    double newThreshold = (mean1 + mean2) / 2;
    delta = std::abs(newThreshold - threshold);
    threshold = newThreshold;
}
cv::threshold(src, dst, threshold, 255, cv::THRESH_BINARY);

// K-mean clustering threshold

cv::Mat src, dst;
double maxValue = 255;
int numClusters = 2; // number of clusters
cv::Mat samples(src.rows * src.cols, 1, CV_32F);
int sampleIdx = 0;
for (int i = 0; i < src.rows; i++) {
    for (int j = 0; j < src.cols; j++) {
        samples.at<float>(sampleIdx++) = src.at<uchar>(i, j);
    }
}
cv::Mat labels, centers;
cv::kmeans(samples, numClusters, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);
int threshold = (int)centers.at<float>(0, 0); // use the minimum cluster center as threshold
if (centers.at<float>(1, 0) < threshold) threshold = (int)centers.at<float>(1, 0); // update the threshold if there is a lower center
cv::threshold(src, dst, threshold, 255, cv::THRESH_BINARY);


//Otsu thresholding

cv::Mat src, dst;
cv::threshold(img, dst, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

