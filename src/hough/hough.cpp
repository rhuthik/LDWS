#include <constants.hpp>
#include <hough/hough.hpp>
#include <hough/hough_prob.hpp>
#include <hough/hough_normal.hpp>
#include <hough/hough_adaptive.hpp>

hough_prob object_prob;
hough_normal object_normal;
hough_adaptive object_adaptive;

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
    std::chrono::microseconds temp_worst_case_time(0);
    cv::Mat input_image = cv::imread("../../dataset/initialization/road.jpg");
    cv::Mat hough_image;
    if (this->is_variant(hough_selection))
    {
        for (int execution_count = 0; execution_count < INITIAL_EXECUTION_COUNT; execution_count++)
        {
            // cout << "Iteration Number: " << execution_count << endl;
            std::chrono::microseconds current_execution_time = temp_worst_case_time;
            this->process(input_image, hough_image, &current_execution_time, hough_selection);
            if (current_execution_time > temp_worst_case_time)
            {
                temp_worst_case_time = current_execution_time;
            }
        }
    }
    // cout << "Worst Execution Time: " << temp_worst_case_time.count() << endl;
    return temp_worst_case_time;
}

void detect_hough::process(cv::Mat input, cv::Mat &output, std::chrono::microseconds *tick_count, int hough_selection)
{
    auto start_time = std::chrono::steady_clock::now();
    if (hough_selection == 0) // Probalistic Hough
    {
        object_prob.process(input, output);
    }
    else if (hough_selection == 1) // Normal
    {
        object_normal.process(input, output);
    }
    else if (hough_selection == 2) // Adaptive
    {
        object_adaptive.process(input, output);
    }

    auto end_time = std::chrono::steady_clock::now();

    // Calculate elapsed time in microsecs
    *tick_count = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
}
