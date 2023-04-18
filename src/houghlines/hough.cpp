#include <constants.hpp>
#include <houghlines/hough.hpp>
#include <houghlines/hough_prob.hpp>
#include <houghlines/hough_normal.hpp>
#include <houghlines/hough_adaptive.hpp>

hough_prob object_prob;
hough_normal object_normal;
hough_adaptive object_adaptive_hough;

bool detect_hough::is_variant(int hough_selection)
{

    if (hough_selection >= NUM_HOUGH_VARIANTS)
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::chrono::microseconds detect_hough::get_tick_count(int hough_selection)
{
    cv::Mat input_image_gray;
    cv::Mat input_image = cv::imread("../../dataset/initialization/edges.jpg");
    cv::cvtColor(input_image, input_image_gray, cv::COLOR_BGR2GRAY);
    input_image = cv::imread("../../dataset/initialization/road.jpg");
    std::chrono::microseconds temp_worst_case_time(0);
    if (this->is_variant(hough_selection))
    {
        for (int execution_count = 0; execution_count < INITIAL_EXECUTION_COUNT; execution_count++)
        {
            // cout << "Iteration Number: " << execution_count << endl;
            cv::Mat hough_image;
            std::chrono::microseconds current_execution_time = temp_worst_case_time;
            hough_image=this->process(input_image_gray,input_image, &current_execution_time, hough_selection);
            std::string filename = "../../dataset/output/hough/hough" + std::to_string(hough_selection) + ".jpg";
            cv::imwrite(filename, hough_image);
            if (hough_image.empty())
            {
                std::cout << "Could not read blurred image" << std::endl;
            }
            if (current_execution_time > temp_worst_case_time)
            {
                temp_worst_case_time = current_execution_time;
            }
        }
    }
    // cout << "Worst Execution Time: " << temp_worst_case_time.count() << endl;
    return temp_worst_case_time;
}

cv::Mat detect_hough::process(cv::Mat& input,cv::Mat input_image, std::chrono::microseconds *tick_count, int hough_selection)
{
    cv::Mat output;
    auto start_time = std::chrono::steady_clock::now();
    if (hough_selection == 0) // Probalistic Hough
    {
        output=object_prob.process(input,input_image);
    }
    else if (hough_selection == 1) // Normal
    {
        output=object_normal.process(input,input_image);
    }
    else if (hough_selection == 2) // Adaptive
    {
        output=object_adaptive_hough.process(input,input_image);
    }

    auto end_time = std::chrono::steady_clock::now();

    // Calculate elapsed time in microsecs
    *tick_count = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return output;
}
