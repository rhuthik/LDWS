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
    // cv::Mat input_image_gray;
    cv::Mat input_image_gray = cv::imread("../../dataset/initialization/blur.png",cv::IMREAD_GRAYSCALE);
    // cv::cvtColor(input_image,input_image_gray,cv::COLOR_BGR2GRAY);
    cv::Mat thresholded_image;
    if(this->is_variant(threshold_selection))
    {
        for(int execution_count = 0; execution_count < INITIAL_EXECUTION_COUNT; execution_count++)
        {
            //cout << "Iteration Number: " << execution_count << endl;
            std::chrono::microseconds current_execution_time = temp_worst_case_time;
            thresholded_image=this->process(input_image_gray, &current_execution_time, threshold_selection);
            std::string filename = "../../dataset/output/threshold/threshold" + std::to_string(threshold_selection) + ".jpg";
            cv::imwrite(filename, thresholded_image);
            if(current_execution_time > temp_worst_case_time)
            {
                temp_worst_case_time = current_execution_time;
            }
        }

    }
    //cout << "Worst Execution Time: " << temp_worst_case_time.count() << endl;
    return temp_worst_case_time;
}

cv::Mat threshold_image::process(cv::Mat& input, std::chrono::microseconds* tick_count, int threshold_selection)
{
    cv::Mat output;
    auto start_time = std::chrono::steady_clock::now();
    if(threshold_selection == 0) // Otsu Thresholding
    {
        output=object_otsu.process(input);       
    }
    else if(threshold_selection == 1) // IsoData Thresholding
    {
        output=object_isodata.process(input);     
    }
    else if(threshold_selection == 2) // Adaptive Thresholding
    {
        output=object_adaptive.process(input);     
    }
    else if(threshold_selection == 3) // MinError Thresholding
    {
        output=object_minerror.process(input);     
    }
    else if(threshold_selection == 4) // MaxEntropy Thresholding
    {
        output=object_maxentropy.process(input);     
    }
    else if(threshold_selection == 5) // KMeanClustering Thresholding
    {
        output=object_kmclustering.process(input);     
    }

    auto end_time = std::chrono::steady_clock::now();

    // Calculate elapsed time in microsecs
    *tick_count = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return output;
}
