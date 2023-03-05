#include <constants.hpp>
#include <blurring/blur.hpp>
#include <blurring/blur_box.hpp>
#include <blurring/blur_median.hpp>
#include <blurring/blur_gaussian.hpp>
#include <blurring/blur_histogram.hpp>

blur_box object_box;
blur_gaussian object_gaussian;
blur_histogram object_histogram;
blur_median object_median;


bool blur_image::is_variant(int blur_selection)
{

    if(blur_selection >= NUM_BLUR_VARIANTS)
    {
        return false;
    }
    else
    {
        return true;
    }

}

std::chrono::microseconds blur_image::get_tick_count(int blur_selection)
{
    std::chrono::microseconds temp_worst_case_time (0);
    cv::Mat input_image = cv::imread("../../dataset/initialization/road.jpg");
    cv::Mat blurred_image;
    if(this->is_variant(blur_selection))
    {
        for(int execution_count = 0; execution_count < INITIAL_EXECUTION_COUNT; execution_count++)
        {
            cout << "Iteration Number: " << execution_count << endl;
            std::chrono::microseconds current_execution_time = temp_worst_case_time;
            this->process(input_image, blurred_image, &current_execution_time, blur_selection);
            if(current_execution_time > temp_worst_case_time)
            {
                temp_worst_case_time = current_execution_time;
            }
        }

    }
    cout << "Worst Execution Time: " << temp_worst_case_time.count() << endl;
    return temp_worst_case_time;
}

void blur_image::process(cv::Mat input,cv::Mat& output, std::chrono::microseconds* tick_count, int blur_selection)
{
    auto start_time = std::chrono::steady_clock::now();
    if(blur_selection == 0) // box blurring
    {
        object_box.process(input, output);       
    }
    else if(blur_selection == 1) // gaussian blurring
    {
        object_gaussian.process(input, output);     
    }
    else if(blur_selection == 2) // histogram blurring
    {
        object_histogram.process(input, output);     
    }
    else if(blur_selection == 3) // median blurring
    {
        object_median.process(input, output);     
    }

    auto end_time = std::chrono::steady_clock::now();

    // Calculate elapsed time in microsecs
    *tick_count = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

}
