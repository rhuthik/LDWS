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
    cv::Mat input_image = cv::imread("/home/cps/ldws/dataset/initialization/road.jpg");
    cv::Mat input_image_gray;
    cv::cvtColor(input_image, input_image_gray, cv::COLOR_BGR2GRAY);
    cv::Mat blurred_image;
    if(this->is_variant(blur_selection))
    {
        for(int execution_count = 0; execution_count < INITIAL_EXECUTION_COUNT; execution_count++)
        {
            //cout << "Iteration Number: " << execution_count << endl;
            std::chrono::microseconds current_execution_time = temp_worst_case_time;
            blurred_image=this->process(input_image_gray, &current_execution_time, blur_selection);
            std::string filename = "../../dataset/output/blur/blur" + std::to_string(blur_selection) + ".jpg";
            cv::imwrite(filename, blurred_image);
            if (blurred_image.empty())
            {
                std::cout << "Could not read blurred image" << std::endl;
            }
            if(current_execution_time > temp_worst_case_time)
            {
                temp_worst_case_time = current_execution_time;
            }
        }

    }
    //cout << "Worst Execution Time: " << temp_worst_case_time.count() << endl;
    return temp_worst_case_time;
}

cv::Mat blur_image::process(cv::Mat& input, std::chrono::microseconds* tick_count, int blur_selection)
{
    cv::Mat output;
    auto start_time = std::chrono::steady_clock::now();
    if(blur_selection == 0) // box blurring
    {
        output=object_box.process(input);       
    }
    else if(blur_selection == 1) // gaussian blurring
    {
        output=object_gaussian.process(input);     
    }
    else if(blur_selection == 2) // histogram blurring
    {
        output=object_histogram.process(input);     
    }
    else if(blur_selection == 3) // median blurring
    {
        output=object_median.process(input);     
    }
    auto end_time = std::chrono::steady_clock::now();

    // Calculate elapsed time in microsecs
    *tick_count = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return output;
}
