#include <constants.hpp>
#include <edgedetection/edge.hpp>
#include <edgedetection/edge_canny.hpp>
#include <edgedetection/edge_sobel.hpp>
#include <edgedetection/edge_laplacian.hpp>

edge_canny object_canny;
edge_laplacian object_laplacian;
edge_sobel object_sobel;

bool detect_edges::is_variant(int edge_detection_selection)
{

    if (edge_detection_selection >= NUM_EDGE_DETECTION_VARIANTS)
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::chrono::microseconds detect_edges::get_tick_count(int edge_detection_selection)
{
    std::chrono::microseconds temp_worst_case_time(0);
    cv::Mat input_image = cv::imread("../../dataset/initialization/threshold.jpg");
    cv::Mat input_image_gray;
    cv::cvtColor(input_image, input_image_gray, cv::COLOR_BGR2GRAY);
    cv::Mat edge_image;
    if (this->is_variant(edge_detection_selection))
    {
        for (int execution_count = 0; execution_count < INITIAL_EXECUTION_COUNT; execution_count++)
        {
            // cout << "Iteration Number: " << execution_count << endl;
            std::chrono::microseconds current_execution_time = temp_worst_case_time;
            edge_image=this->process(input_image_gray, &current_execution_time, edge_detection_selection);
            std::string filename = "../../dataset/output/edge/edge" + std::to_string(edge_detection_selection) + ".jpg";
            cv::imwrite(filename, edge_image);
            if (edge_image.empty())
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

cv::Mat detect_edges::process(cv::Mat& input, std::chrono::microseconds *tick_count, int edge_detection_selection)
{
    cv::Mat output;
    auto start_time = std::chrono::steady_clock::now();
    if (edge_detection_selection == 0) // Canny Edge
    {
        // cout<<"reached canny"<<endl;
        output=object_canny.process(input);
    }
    else if (edge_detection_selection == 1) // Laplacian Edge
    {
        // cout<<"reached lap"<<endl;
        output=object_laplacian.process(input);
    }
    else if (edge_detection_selection == 2) // Sobel Edge
    {
        // cout<<"reached sobel"<<endl;
        output=object_sobel.process(input);
    }
    else{
        cout<<edge_detection_selection<<"\n";
    }
    auto end_time = std::chrono::steady_clock::now();

    // Calculate elapsed time in microsecs
    *tick_count = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return output;
}
