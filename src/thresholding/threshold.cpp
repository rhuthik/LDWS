#include <constants.hpp>
#include <thresholding/threshold.hpp>
#include <thresholding/thresholding_otsu.hpp>
#include <thresholding/thresholding_isodata.hpp>
#include <thresholding/thresholding_adaptive.hpp>
#include <thresholding/thresholding_minerror.hpp>
#include <thresholding/thresholding_maxentropy.hpp>
#include <thresholding/thresholding_kmclustering.hpp>


threshold_otsu object_otsu;
threshold_isodata object_isodata;
threshold_adaptive object_adaptive;
threshold_minerror object_minerror;
threshold_maxentropy object_maxentropy;
threshold_kmclustering object_kmclustering;


bool threshold_image::is_variant(int threshold_selection)
{

    if(threshold_selection >= NUM_THRESHOLD_VARIANTS)
    {
        return false;
    }
    else
    {
        return true;
    }

}

std::chrono::microseconds threshold_image::get_tick_count(int threshold_selection)
{
    std::chrono::microseconds temp_worst_case_time (0);
    cv::Mat input_image = cv::imread("../../dataset/initialization/blurred.png");
    cv::Mat thresholded_image;
    if(this->is_variant(threshold_selection))
    {
        for(int execution_count = 0; execution_count < INITIAL_EXECUTION_COUNT; execution_count++)
        {
            //cout << "Iteration Number: " << execution_count << endl;
            std::chrono::microseconds current_execution_time = temp_worst_case_time;
            this->process(input_image, thresholded_image, &current_execution_time, threshold_selection);
            if(current_execution_time > temp_worst_case_time)
            {
                temp_worst_case_time = current_execution_time;
            }
        }

    }
    //cout << "Worst Execution Time: " << temp_worst_case_time.count() << endl;
    return temp_worst_case_time;
}

void threshold_image::process(cv::Mat input,cv::Mat& output, std::chrono::microseconds* tick_count, int threshold_selection)
{
    auto start_time = std::chrono::steady_clock::now();
    if(threshold_selection == 0) // Otsu Thresholding
    {
        object_otsu.process(input, output);       
    }
    else if(threshold_selection == 1) // IsoData Thresholding
    {
        object_isodata.process(input, output);     
    }
    else if(threshold_selection == 2) // Adaptive Thresholding
    {
        object_adaptive.process(input, output);     
    }
    else if(threshold_selection == 3) // MinError Thresholding
    {
        object_minerror.process(input, output);     
    }
    else if(threshold_selection == 4) // MaxEntropy Thresholding
    {
        object_maxentropy.process(input, output);     
    }
    else if(threshold_selection == 5) // KMeanClustering Thresholding
    {
        object_kmclustering.process(input, output);     
    }

    auto end_time = std::chrono::steady_clock::now();

    // Calculate elapsed time in microsecs
    *tick_count = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

}
