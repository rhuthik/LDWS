#include <iostream>
#include <random>
#include <dirent.h>
#include <fstream>
#include <opencv2/opencv.hpp>

// Import Modules
#include <constants.hpp>
#include <shedutil/schedule.hpp>
#include <blurring/blur.hpp>
#include <thresholding/threshold.hpp>
#include <edgedetection/edge.hpp>
#include <houghlines/hough.hpp>
#include <string.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace cv;
using namespace std;

int image_index = 0;
int counting = 000000;
// int energy[18];
// double accuracyarr[18];

// Create a Module Object
blur_image blur_image_instance;
detect_edges detect_edges_instance;
threshold_image threshold_image_instance;
detect_hough hough_instance;

schedule::schedule(double redundancy_threshold, float quality_threshold,
                   long long int required_fps, long long int total_energy_budget,
                   String update_selection_nature, String update_selection_priority,
                   String update_selection_mode, String initial_selection_mode)
{
    this->redundancy_threshold = redundancy_threshold;
    this->quality_threshold = quality_threshold;
    this->required_fps = required_fps;
    this->total_energy_budget = total_energy_budget;
    this->update_selection_nature = update_selection_nature;
    this->update_selection_priority = update_selection_priority;
    this->update_selection_mode = update_selection_mode;
    this->initial_selection_mode = initial_selection_mode;
    counting = 0;
    image_index = 0;
    return;
}

void schedule::initialize()
{
    for (int module_id = 0; module_id < MODULE_COUNT; module_id++)
    {
        this->schedule_matrix.push_back(vector<Element>());
        int variant_id = 0;
        while (true)
        {
            std::chrono::microseconds worst_case_execution_energy;
            cout << module_id << endl;
            if (module_id == 0 && blur_image_instance.is_variant(variant_id)) // Deblurring
                worst_case_execution_energy = blur_image_instance.get_tick_count(variant_id);

            else if (module_id == 1 && threshold_image_instance.is_variant(variant_id)) // Thresholding
                worst_case_execution_energy = threshold_image_instance.get_tick_count(variant_id);

            else if (module_id == 2 && detect_edges_instance.is_variant(variant_id)) // Edge Detection
                worst_case_execution_energy = detect_edges_instance.get_tick_count(variant_id);

            else if (module_id == 3 && hough_instance.is_variant(variant_id)) // Line Detection (Hough)
                worst_case_execution_energy = hough_instance.get_tick_count(variant_id);

            else
                break;

            // Run each variant mutiple times and push back the tuple with initial values
            // cout << "Module ID: " << module_id << " Variant ID: " << variant_id << " with worst case execution time: " << worst_case_execution_energy.count() << " us" << endl;
            this->schedule_matrix[module_id].push_back(Element{module_id, variant_id, worst_case_execution_energy, 0.0});
            variant_id++;
        }
        if (this->max_module_length < variant_id)
            this->max_module_length = variant_id;
        // cout << "abcd" << endl;
    }
    // //cout << "here" << endl;;
    //  before Sorting the values
    // this->print();

    // Sort the techniques across each row
    for (std::vector<int>::size_type i = 0; i < this->schedule_matrix.size(); i++)
        sort(this->schedule_matrix[i].begin(), this->schedule_matrix[i].end(), compare_energy);

    // Print the sorted elements of the matrix
    // this->print();

    // Initial Selection
    // Three variants Right most, leftmost, center, random
    for (std::vector<int>::size_type i = 0; i < this->schedule_matrix.size(); i++)
    {
        int variant_selection = 0;
        int size = static_cast<int>(this->schedule_matrix[static_cast<int>(i)].size());
        if (initial_selection_mode == "left")
        {
            variant_selection = 0;
        }
        else if (initial_selection_mode == "right")
        {
            variant_selection = size - 1;
        }
        else if (initial_selection_mode == "center_right")
        {
            if (size % 2 == 0)
                variant_selection = size / 2 + 1 - 1; //+1 for right element and -1 to adjust index as it starts from 0
            else
                variant_selection = ((size + 2 - 1) / 2) - 1;
        }
        else if (initial_selection_mode == "center_left")
        {
            if (size % 2 == 0)
                variant_selection = size / 2 - 1; //+1 for right element and -1 to adjust index as it starts from 0
            else
                variant_selection = ((size + 2 - 1) / 2) - 1;
        }
        else
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<int> choices{0, size - 1, ((size + 2 - 1) / 2) - 1};
            std::uniform_int_distribution<> dist(0, 2);
            variant_selection = choices[dist(gen)];
        }
        this->selection.push_back(make_pair(static_cast<int>(i), variant_selection));
        this->selection_length++;
        // this->relative_selection.push_back(this->schedule_matrix[static_cast<int>(i)][variant_selection]);
        // cout << variant_selection << " ";
    }

    // Other Initializations

    // 1. Run the selection and note down the individual cycles

    // Initialize the dataset files
    this->image_files = read_images_in_directory("../../dataset/inputs/ktest");
    this->reference_image_files = read_images_in_directory("../../dataset/references/ktest");

    // 2. Compute and update Worst Case Execution Cycles and Accuracy

    // 3. Check for redundancy, update process statistics and load the image to be processed

    // 4. Compute energy budgets

    // Computations per component
    std::chrono::microseconds initial_budget(0);
    this->time_run_selections = initial_budget;
    this->time_update_accuracy = initial_budget;
    this->time_update_redundancy = initial_budget;
    this->time_energy_budget = initial_budget;
    this->time_generate_selections = initial_budget;
    this->time_update_selections = initial_budget;

    // Calculate System Capacity
    std::chrono::microseconds capacity(100000000 / this->required_fps);
    this->system_capacity = capacity.count();
    // total_image_count
    this->total_image_count = static_cast<long long int>(this->image_files.size());

    // 5. Generate feasible selections

    // 6. Update selection

    // 7. Logging

    // Build Path
    String storage_path = this->dataset;
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + initial_selection_mode;
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + to_string(this->required_fps) + "_fps";
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + to_string(this->total_energy_budget) + "_ebudget";
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + this->update_selection_mode;
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + this->update_selection_priority;
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + this->update_selection_nature;
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + to_string(this->redundancy_threshold) + "_r_thres";
    mkdir(storage_path.c_str(), 0777);
    storage_path += "/" + to_string(this->quality_threshold) + "_q_thres";
    mkdir(storage_path.c_str(), 0777);

    // Data File
    this->data_file.open(storage_path + "/output.csv");

    // Columns
    this->data_file << "energy_run_selection, energy_update_accuracy, energy_redundancy_check, energy_cal_budget, energy_gen_selections, energy_update_selection, current_computation_energy, available_energy_per_image, total_system_capacity_available, total_budget_available, current_accuracy, changed_module_id, changed_variant_id, images_remaining, number_of_available_selections \n";

    // Initial Section and Schedule Matrix
    this->txt_file.open(storage_path + "/selection_data.txt");
    this->txt_file << "Initialization Stage: \n"
                   << endl;
    this->log("initial");
    this->txt_file << "Run Time Stage: \n"
                   << endl;
    for (int i = 0; i < 4; i++)
    {
        cout << "intital selection:" << endl;
        std::cout << "Index: " << i << ", Value: (" << this->selection[i].first << ", " << this->selection[i].second << ")" << std::endl;
    }

    return;
}

void schedule::run()
{
    // int c=0;
    while (true)
    { // 1. Run the selection and note down the individual cycles
        cv::Mat processed_image = this->run_selection();

        //  Breaks if there is nothing to process
        if (this->read_error_flag)
        {
            this->txt_file << "No more images to read\n"
                           << endl;
            break;
        }

        //  2. Compute and update Worst Case Execution Cycles and Accuracy
        this->update_worst_case_cycles_quality(processed_image);

        //  3. Check for redundancy, update process statistics and load the image to be processed
        this->check_redundancy_statistics();
        if (this->read_error_flag)
        {
            this->txt_file << "No more images to read\n"
                           << endl;
            break;
        }

        //  4. Compute energy budgets
        this->compute_energy_budgets();
        // cout << "4\n";
        if (this->read_error_flag)
        {
            this->txt_file << "System out of energy\n"
                           << endl;
            break;
        }

        //  5. Generate feasible selections
        this->generate_feasible_selections();
        // cout << "5\n";

        //  6. Update selection
        this->update_selection_set();

        // 7. Log the values
        this->log();

        this->print();
    }
    this->txt_file.close();
    return;
}

cv::Mat schedule::run_selection() // 1
{
    cv::Mat input_image = get_next_image(this->image_files, true, "current image for processing"); // image_files, is_current_image, is_reference
    // cout << "IMAGE IS "<< input_image << "\n";
    if (input_image.empty())
    {
        // cout << "no image" << endl;
        this->read_error_flag = true;
        return cv::Mat();
    }
    cv::Mat blurred_image;
    cv::Mat edge_detected_image;
    cv::Mat threshold_image;
    cv::Mat Hough_image_output;
    this->execution_costs.clear();

    // Blurring
    std::chrono::microseconds execution_cycles_blur(0);
    blurred_image = blur_image_instance.process(input_image, &execution_cycles_blur, this->selection[0].second);
    this->execution_costs.push_back(execution_cycles_blur);
    if (blurred_image.empty())
    {
        std::cout << "empty" << std::endl;
    }
    std::string filename = "../../dataset/results/blur/blur" + std::to_string(counting) + ".jpg";
    cv::imwrite(filename, blurred_image);

    // Thresholding
    std::chrono::microseconds execution_cycles_thresholding(0);
    threshold_image = threshold_image_instance.process(blurred_image, &execution_cycles_thresholding, this->selection[1].second);
    this->execution_costs.push_back(execution_cycles_thresholding);
    filename = "../../dataset/results/thresh/threshold" + std::to_string(counting) + ".jpg";
    cv::imwrite(filename, threshold_image);

    // Edge Detection
    std::chrono::microseconds execution_cycles_edge(0);
    edge_detected_image = detect_edges_instance.process(threshold_image, &execution_cycles_edge, this->selection[2].second);
    this->execution_costs.push_back(execution_cycles_edge);
    filename = "../../dataset/results/edge/edge" + std::to_string(counting) + ".jpg";
    cv::imwrite(filename, edge_detected_image);
    // cout<<"after edge\n";

    // Hough
    std::chrono::microseconds execution_cycles_hough(0);
    cv::Mat image = cv::imread(image_files[image_index]);
    Hough_image_output = hough_instance.process(edge_detected_image, image, &execution_cycles_hough, this->selection[3].second);
    this->execution_costs.push_back(execution_cycles_hough);
    filename = "../../dataset/results/hough/hough" + std::to_string(counting) + ".jpg";
    cv::imwrite(filename, Hough_image_output);
    std::chrono::microseconds initial_budget(0);
    this->time_run_selections = accumulate(this->execution_costs.begin(), this->execution_costs.end(), initial_budget);

    counting += 1;
    return Hough_image_output;
}

void schedule::update_worst_case_cycles_quality(cv::Mat output_image) // 2
{
    auto start_time = std::chrono::steady_clock::now();
    // Accuracy Calculation

    cv::Mat reference_image = get_next_image(this->reference_image_files, true, " reference image for quality");
    // cv::Mat difference_image;
    int N = output_image.rows * output_image.cols;
    // cv::absdiff(output_image, reference_image, difference_image);
    // double mse = cv::mean(difference_image.mul(difference_image))[0];
    double mse = 0;
    for (int i = 0; i < output_image.rows; i++)
    {
        for (int j = 0; j < output_image.cols; j++)
        {
            double diff = output_image.at<uchar>(i, j) - reference_image.at<uchar>(i, j);
            mse += diff * diff;
        }
    }
    mse /= N;
    // Calculate the PSNR between the two images
    double psnr = 10 * log10(255 * 255 / mse);
    // Calculate the percentage of accuracy
    double max_psnr = 20 * log10(255);
    double accuracy = (psnr / max_psnr) * 100;
    this->current_quality = accuracy;

    // Update Worst Case Cycles and accuracy
    for (int i = 0; i < this->selection_length; i++)
    {
        auto element = this->selection[i];
        int module_id = element.first;
        int actual_variant_id = this->get_actual_position(module_id, element.second);
        if (this->schedule_matrix[module_id][actual_variant_id].energy.count() < this->execution_costs[i].count())
            this->schedule_matrix[module_id][actual_variant_id].energy = this->execution_costs[i];

        // Quality update TODO: Moving average??
        if (this->schedule_matrix[module_id][actual_variant_id].quality == 0)
        {
            this->schedule_matrix[module_id][actual_variant_id].quality = this->current_quality;
        }
        else
        {
            this->schedule_matrix[module_id][actual_variant_id].quality = (this->schedule_matrix[module_id][actual_variant_id].quality + this->current_quality) / 2;
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    this->accuracy_per_image.push_back(this->current_quality);
    this->time_update_accuracy = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << accuracy << " accuracy\n"
         << endl;
    // accuracyarr[counting] = accuracy;
    this->print();
    cout << "accuracy updated" << endl;
    return;
}

void schedule::check_redundancy_statistics() // 3
{
    vector<std::chrono::microseconds> time_temp_vector;

    bool redundancy = false;

    while (!redundancy && !this->read_error_flag && (this->total_image_count > 0))
    {
        redundancy = false;

        cv::Mat current_image = get_next_image(this->image_files, true, " current image for redundancy check");
        // cv::cvtColor(current_image, greyMat, CV_BGR2GRAY);
        cv::Mat next_image = get_next_image(this->image_files, false, " next image for redundancy check");

        if (current_image.empty())
        {
            cout << "no curr img";
            this->read_error_flag = true;
            break;
        }
        if (next_image.empty())
        {
            cout << "no next image\n";
            this->read_error_flag = true;
            break;
        }

        // cv::Mat difference_image;

        // Start the time
        auto start_time = std::chrono::steady_clock::now();
        int N = current_image.rows * current_image.cols;
        // cv::absdiff(current_image, next_image, difference_image);
        // double mse = cv::mean(difference_image.mul(difference_image))[0];
        double mse = 0;
        for (int i = 0; i < current_image.rows; i++)
        {
            for (int j = 0; j < current_image.cols; j++)
            {
                double diff = current_image.at<uchar>(i, j) - next_image.at<uchar>(i, j);
                mse += diff * diff;
            }
        }
        mse /= N;
        // Calculate the PSNR between the two images
        double psnr = 10 * log10(255 * 255 / mse);
        // Calculate the percentage of accuracy
        double max_psnr = 20 * log10(255);
        double similarity = (psnr / max_psnr) * 100;
        if (similarity < this->redundancy_threshold)
        {
            redundancy = true;
        }
        else // update statistics like image count etc.
        {
            // cout<<"redundant image, updating total_image_count";
            this->total_image_count -= 1;
        }

        if (this->total_image_count <= 0)
        {
            this->read_error_flag = true;
            auto end_time = std::chrono::steady_clock::now();
            time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time));
            break;
        }
        // End the timer and record the values
        auto end_time = std::chrono::steady_clock::now();
        time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time));

        // Prints Not Included in the timing (Extra logic)
        if (similarity < this->redundancy_threshold)
        {
            this->data_file << "0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, " << this->total_image_count << ", 0 \n";
            cout << "redundant image, updating total_image_count";
        }
        cout << "Redundancy value: " << similarity << endl;
        this->txt_file << "Redundant Image:" << endl;
    }

    std::chrono::microseconds initial_budget(0);
    this->time_update_redundancy = accumulate(time_temp_vector.begin(), time_temp_vector.end(), initial_budget);

    return;
}

void schedule::compute_energy_budgets() // 4
{
    auto start_time = std::chrono::steady_clock::now();
    // remove computation losses
    std::chrono::microseconds initial_budget(0);
    std::chrono::microseconds computation_budget = this->time_run_selections +
                                                   this->time_update_accuracy + this->time_update_redundancy +
                                                   this->time_energy_budget + this->time_generate_selections +
                                                   this->time_update_selections;
    long long int left_over_budget = this->total_energy_budget - computation_budget.count();
    this->total_energy_budget = this->total_energy_budget - computation_budget.count();
    // cout <<"lob"<< left_over_budget << endl;

    // Get the budget per image based on left over budget and calculate if system needs to save energy and also calculate available budget

    long long int calculated_budget_per_image = left_over_budget / this->total_image_count;

    // available extra budget for state change
    long long int calculated_budget_for_state_change = calculated_budget_per_image - computation_budget.count();
    // available system capacity for state change
    long long int system_capacity_for_processing = this->system_capacity - computation_budget.count();
    std::chrono::microseconds temp_budget(min(calculated_budget_for_state_change, system_capacity_for_processing));
    this->available_budget = temp_budget;

    if (this->available_budget.count() <= 0)
        this->save_energy = true;
    else
        this->save_energy = false;
    auto end_time = std::chrono::steady_clock::now();
    this->time_energy_budget = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    // cout << this->available_budget.count() << endl;
    std::cout << "computation energy: " << computation_budget.count() << "\n";
    std::cout << "available energy: " << available_budget.count() << "\n";
    this->energy_per_image.push_back(computation_budget.count());
    // energy[counting] = computation_budget.count();
    if (left_over_budget <= 0)
    {
        this->read_error_flag = true;
    }
    return;
}

void schedule::generate_feasible_selections() // 5
{
    vector<std::chrono::microseconds> time_temp_vector;

    auto start_time_1 = std::chrono::steady_clock::now();
    this->relative_selection.clear();
    auto end_time_1 = std::chrono::steady_clock::now();
    time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_1 - start_time_1));
    int deviation_factor = 1;

    while (static_cast<int>(this->relative_selection.size()) < 1)
    {

        for (int i = 0; i < this->selection_length; i++)
        {
            // Look at left side always
            auto start_time_2 = std::chrono::steady_clock::now();

            auto current_selection = this->selection[i];
            int actual_variant_id = this->get_actual_position(current_selection.first, current_selection.second);
            auto condition_0 = (actual_variant_id - deviation_factor >= 0);

            auto end_time_2 = std::chrono::steady_clock::now();
            time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_2 - start_time_2));
            if (condition_0)
            {
                auto start_time_3 = std::chrono::steady_clock::now();
                auto left_element = this->schedule_matrix[current_selection.first][actual_variant_id - deviation_factor];
                auto current_element = this->schedule_matrix[current_selection.first][actual_variant_id];
                auto condition_1 = ((left_element.quality == 0) || (left_element.quality >= this->quality_threshold)); // constraint on quality
                auto end_time_3 = std::chrono::steady_clock::now();

                time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_3 - start_time_3));

                if (condition_1)
                {
                    auto start_time_4 = std::chrono::steady_clock::now();
                    auto condition_2 = ((left_element.energy.count() - current_element.energy.count()) <= this->available_budget.count()); //constraint on energy
                    auto end_time_4 = std::chrono::steady_clock::now();
                    time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_4 - start_time_4));

                    // if (this->compute_energy_feasibility(left_element, this->available_budget))
                    if (condition_2)
                    {
                        auto start_time_5 = std::chrono::steady_clock::now();

                        this->relative_selection.push_back(left_element);
                        if (left_element.quality == 0)
                            this->unknown_variant_flag = true;

                        auto end_time_5 = std::chrono::steady_clock::now();
                        time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_5 - start_time_5));
                    }
                    else // rejected due to energy
                    {
                        std::cout << "not feasible wrt energy" << endl;
                    }
                }
                else // rejected due to quality
                {
                    std::cout << "not feasible wrt quality" << endl;
                }
            }
            else // boundary condition
            {
                // Handle out-of-range access to schedule_matrix
                std::cout << "Current element is left most element of the row " << current_selection.first
                          << ", column " << actual_variant_id << std::endl;
            }
            // TODO: check if there is atleast one permutation for changing.

            // Right Side -- Process only if there is energy
            if ((actual_variant_id + deviation_factor - 1 < (static_cast<int>(this->schedule_matrix[current_selection.first].size()) - 1)) && (!this->save_energy))
            {
                std::cout << "Enough Energy to Process Right side as well" << endl;

                auto start_time_6 = std::chrono::steady_clock::now();
                auto current_element = this->schedule_matrix[current_selection.first][actual_variant_id];
                auto right_element = this->schedule_matrix[current_selection.first][actual_variant_id + deviation_factor];
                auto condition_3 = ((right_element.energy.count() - current_element.energy.count()) <= this->available_budget.count());
                auto end_time_6 = std::chrono::steady_clock::now();
                time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_6 - start_time_6));

                if (condition_3)
                {
                    auto start_time_7 = std::chrono::steady_clock::now();
                    auto condition_4 = ((right_element.quality == 0) || (right_element.quality >= this->quality_threshold));
                    auto end_time_7 = std::chrono::steady_clock::now();
                    time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_7 - start_time_7));
                    if (condition_4)
                    {
                        auto start_time_8 = std::chrono::steady_clock::now();
                        this->relative_selection.push_back(right_element);
                        if (right_element.quality == 0)
                            this->unknown_variant_flag = true;
                        auto end_time_8 = std::chrono::steady_clock::now();
                        time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time_8 - start_time_8));
                    }
                    else
                    {
                        std::cout << "failed wrt quality--right" << endl;
                    }

                    // add else for retesting and giving another chance for band methods as there is sufficient budget
                }
                else
                {
                    std::cout << "failed wrt energy--right" << endl;
                }
            }
            else
            {
                // Handle out-of-range access to schedule_matrix
                std::cout << "Current element is right most element of the row " << current_selection.first
                          << ", column " << actual_variant_id << std::endl;
            }
        }
        deviation_factor++;
        if (deviation_factor >= (this->max_module_length + 1) / 2)
            break;
    }

    std::chrono::microseconds initial_budget(0);
    for (auto it = 0; it < static_cast<int>(time_temp_vector.size()); it++)
        cout << time_temp_vector[it].count() << " ------:----";
    this->time_generate_selections = accumulate(time_temp_vector.begin(), time_temp_vector.end(), initial_budget);
    // this->print("relative_selection");
    // this->print();
    return;
}

void schedule::update_selection_set() // 6
{
    this->print("relative_selection");
    // No selection are available
    int available_selections = static_cast<int>(this->relative_selection.size());

    Element selected_element;
    // Start timer
    auto start_time = std::chrono::steady_clock::now();

    // Check if there are any selections
    if (available_selections <= 0)
    {
        auto end_time_now = std::chrono::steady_clock::now();
        this->time_update_selections = std::chrono::duration_cast<std::chrono::microseconds>(end_time_now - start_time);
        cout << "Not selecting anything" << endl;
        for (int i = 0; i < 4; i++)
        {
            std::cout << "Index: " << i << ", Value: (" << this->selection[i].first << ", " << this->selection[i].second << ")" << std::endl;
        }
        this->unknown_variant_flag = false;
        return;
    }

    // Sort Based on Priority of Energy/Quality
    if (this->update_selection_priority == "energy")
        sort(this->relative_selection.begin(), this->relative_selection.end(), compare_energy);
    else if (this->update_selection_priority == "quality")
        sort(this->relative_selection.begin(), this->relative_selection.end(), compare_quality);

    // Explorative Mode (Works only if there is an unknown variant)

    // Explorative Energy/Quality
    if (this->unknown_variant_flag && (this->update_selection_nature == "explorative") && ((this->update_selection_priority == "energy") || (this->update_selection_priority == "quality")))
    {
        // Pick the variant which has energy max/min and quality = 0

        // Max-Explorative
        if (this->update_selection_mode == "max")
        {
            for (int i = available_selections - 1; i >= 0; i--)
            {
                if (this->relative_selection[i].quality == 0)
                {
                    selected_element = this->relative_selection[i];
                    break;
                }
            }
        }

        // Min Explorative
        else if (this->update_selection_mode == "min")
        {
            for (int i = 0; i < available_selections; i++)
            {
                if (this->relative_selection[i].quality == 0)
                {
                    selected_element = this->relative_selection[i];
                    break;
                }
            }
        }
    }
    // Non-Explorative Mode or Normal Mode
    else if ((this->update_selection_priority == "energy") || (this->update_selection_priority == "quality"))
    {
        // Pick the variant which has max/min energy/quality

        // Max- Non-Explorative/Normal
        if (this->update_selection_mode == "max")
        {
            selected_element = this->relative_selection[available_selections - 1];
        }
        // Min Non-Explorative/Normal
        else if (this->update_selection_mode == "min")
        {
            selected_element = this->relative_selection[0];
        }
    }

    this->selection[selected_element.module_id] = make_pair(selected_element.module_id, selected_element.variant_id);
    this->unknown_variant_flag = false;
    auto end_time = std::chrono::steady_clock::now();
    this->time_update_selections = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    for (int i = 0; i < 4; i++)
    {
        std::cout << "Index: " << i << ", Value: (" << this->selection[i].first << ", " << this->selection[i].second << ")" << std::endl;
    }
    this->selected_module_id = selected_element.module_id;
    this->selected_variant_id = selected_element.variant_id;
    return;
}

// Optional Functions

int schedule::get_actual_position(int module_id, int row_id)
{
    int actual_position = -1;
    for (int i = 0; i < static_cast<int>(this->schedule_matrix[module_id].size()); i++)
    {
        if (this->schedule_matrix[module_id][i].variant_id == row_id)
        {
            actual_position = i;
            break;
        }
    }
    return actual_position;
}

void schedule::get_relative_selections()
{
    // this->left_relative_selection.clear();
    // this->right_relative_selection.clear();
    this->relative_selection.clear();
    for (int i = 0; i < this->selection_length; i++)
    {
        auto current_element = this->selection[i];

        // left element
        if (current_element.second != 0)
        {
            // this->left_relative_selection.push_back(this->schedule_matrix[current_element.first][current_element.second - 1]);
            this->relative_selection.push_back(this->schedule_matrix[current_element.first][current_element.second - 1]);
        }

        // right element
        if (current_element.second < static_cast<int>(this->schedule_matrix[current_element.first].size()) - 1)
        {
            // this->right_relative_selection.push_back(this->schedule_matrix[current_element.first][current_element.second + 1]);
            this->relative_selection.push_back(this->schedule_matrix[current_element.first][current_element.second + 1]);
        }
    }

    // sort(this->left_relative_selection.begin(), this->left_relative_selection.end(), compare_energy);
    // sort(this->right_relative_selection.begin(), this->right_relative_selection.end(), compare_energy);
    sort(this->relative_selection.begin(), this->relative_selection.end(), compare_energy);

    return;
}

void schedule::generate_feasibility_set(std::chrono::microseconds available_budget, float quality_threshold)
{
    // remove elements which do not satisfy the absolute quality threshold
    this->relative_selection.erase(std::remove_if(this->relative_selection.begin(), this->relative_selection.end(), [quality_threshold](Element e)
                                                  { if(e.quality == 0) { return false;} else {return e.quality < quality_threshold;} }),
                                   this->relative_selection.end());

    // remove elements which do not satisfy energy requirments
    this->relative_selection.erase(std::remove_if(this->relative_selection.begin(), this->relative_selection.end(), [this, available_budget](Element e)
                                                  { return !this->compute_energy_feasibility(e, available_budget); }),
                                   this->relative_selection.end());
    return;
}

// utility and other internal functions

void schedule::print(String print_type)
{
    cout << endl;

    if (print_type == "relative_selection")
    {
        for (int i = 0; i < static_cast<int>(this->relative_selection.size()); i++)
        {
            cout << "(";
            cout << this->relative_selection[i].module_id << ", ";
            cout << this->relative_selection[i].variant_id << ", ";
            cout << this->relative_selection[i].energy.count() << ", ";
            cout << this->relative_selection[i].quality << "), ";
        }
    }

    else if (print_type == "matrix")
    {
        for (std::vector<int>::size_type i = 0; i < this->schedule_matrix.size(); i++)
        {
            for (std::vector<int>::size_type j = 0; j < this->schedule_matrix[i].size(); j++)
            {
                cout << "(";
                cout << this->schedule_matrix[i][j].module_id << ", ";
                cout << this->schedule_matrix[i][j].variant_id << ", ";
                cout << this->schedule_matrix[i][j].energy.count() << ", ";
                cout << this->schedule_matrix[i][j].quality << "), ";
            }
            cout << endl;
        }
    }

    cout << endl;

    return;
}

bool schedule::compute_energy_feasibility(Element e, std::chrono::microseconds available_budget)
{
    auto current_selection = this->selection[e.module_id];
    Element selection_element = this->schedule_matrix[current_selection.first][current_selection.second];
    std::chrono::microseconds selection_element_energy;
    selection_element_energy = selection_element.energy;
    auto difference = e.energy - selection_element_energy;
    // //cout << "Module ID: " << e.module_id << " Variant ID: " << e.variant_id << " : " << (difference <= available_budget) << endl;
    return (difference <= available_budget);
}

void schedule::log(String state)
{
    // Log the Schedule Matrix and Selection

    // Relative Selection Set
    this->txt_file << endl;
    this->txt_file << "------------------------------------------------------------------" << endl;
    this->txt_file << "Image Number: " << (static_cast<long long int>(this->image_files.size()) - this->total_image_count) << endl;
    this->txt_file << "Relative Selection: " << endl;
    for (int i = 0; i < static_cast<int>(this->relative_selection.size()); i++)
    {
        this->txt_file << "(";
        this->txt_file << this->relative_selection[i].module_id << ", ";
        this->txt_file << this->relative_selection[i].variant_id << ", ";
        this->txt_file << this->relative_selection[i].energy.count() << ", ";
        this->txt_file << this->relative_selection[i].quality << "), ";
    }

    // Schedule Matrix
    this->txt_file << endl;
    this->txt_file << "Schedule Matrix: " << endl;
    for (std::vector<int>::size_type i = 0; i < this->schedule_matrix.size(); i++)
    {
        for (std::vector<int>::size_type j = 0; j < this->schedule_matrix[i].size(); j++)
        {
            this->txt_file << "(";
            this->txt_file << this->schedule_matrix[i][j].module_id << ", ";
            this->txt_file << this->schedule_matrix[i][j].variant_id << ", ";
            this->txt_file << this->schedule_matrix[i][j].energy.count() << ", ";
            this->txt_file << this->schedule_matrix[i][j].quality << "), ";
        }
        this->txt_file << endl;
    }

    // Selection Set
    this->txt_file << endl;
    this->txt_file << "Selection Set: " << endl;
    for (int i = 0; i < static_cast<int>(this->selection.size()); i++)
    {
        this->txt_file << "Index: " << i << ", Value: (" << this->selection[i].first << ", " << this->selection[i].second << "), ";
    }
    this->txt_file << endl;
    this->txt_file << "------------------------------------------------------------------" << endl;

    // Log the Parameters
    if (state == "initial")
    {
        cout << "Initial_state" << endl;
    }
    else if (state == "run")
    {
        std::chrono::microseconds computation_budget = this->time_run_selections +
                                                       this->time_update_accuracy + this->time_update_redundancy +
                                                       this->time_energy_budget + this->time_generate_selections +
                                                       this->time_update_selections;
        int available_selections = static_cast<int>(this->relative_selection.size());
        this->data_file << this->time_run_selections.count()
                        << ", " << this->time_update_accuracy.count()
                        << ", " << this->time_update_redundancy.count()
                        << ", " << this->time_energy_budget.count()
                        << ", " << this->time_generate_selections.count()
                        << ", " << this->time_update_selections.count()
                        << ", " << computation_budget.count()
                        << ", " << this->available_budget.count()
                        << ", " << this->system_capacity - computation_budget.count()
                        << ", " << this->total_energy_budget
                        << ", " << this->current_quality
                        << ", " << this->selected_module_id
                        << ", " << this->selected_variant_id
                        << ", " << this->total_image_count
                        << ", " << available_selections
                        << endl;
    }
    return;
}

bool compare_energy(const Element &e1, const Element &e2)
{
    return e1.energy < e2.energy;
}

bool compare_quality(const Element &e1, const Element &e2)
{
    return e1.quality < e2.quality;
}

// read Images paths in a directory
vector<std::string> read_images_in_directory(const std::string directory)
{
    static vector<std::string> image_files;
    // open the directory
    DIR *dir = opendir(directory.c_str());
    if (dir == NULL)
    {
        throw runtime_error("Error opening directory " + directory);
    }

    // read all files in the directory and filter out only image files
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string filename = entry->d_name;
        if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".jpg")
        {
            image_files.push_back(directory + "/" + filename);
        }
        else if (filename.length() > 5 && filename.substr(filename.length() - 5) == ".jpeg")
        {
            image_files.push_back(directory + "/" + filename);
        }
        else if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".png")
        {
            image_files.push_back(directory + "/" + filename);
        }
    }
    closedir(dir);
    // sort the image_files vector by filename
    std::sort(image_files.begin(), image_files.end(), [](const string &a, const string &b)
              {
        // extract the numeric part of the filenames
        int a_num = std::stoi(a.substr(a.length() - 9, 5));
        int b_num = std::stoi(b.substr(b.length() - 9, 5));
        return a_num < b_num; });

    // print the file paths in ascending order
    for (auto element : image_files)
    {
        cout << element << endl;
    }
    return image_files;
}

// Read Images

cv::Mat get_next_image(std::vector<std::string> image_files, bool is_current_image, String purpose)
{

    cv::Mat image;
    cout << endl
         << "current_image_index is: " << image_index << " Purpose: " << purpose << endl;
    if (!is_current_image)
    {
        image_index++;
    }

    if (image_index < static_cast<int>(image_files.size()))
    {

        cv::Mat image = cv::imread(image_files[image_index]);

        if (image.empty())
        {
            cout << "Could not read image" << std::endl;
        }
        cv::Mat greyMat;
        cv::cvtColor(image, greyMat, cv::COLOR_BGR2GRAY);
        return greyMat;
    }
    else
    {
        cout << "\n";
        cout << image_index << " " << static_cast<int>(image_files.size()) << "\n";
        cout << "All Images have been processed" << endl;
        /*
        for (int i = 0; i < 18; i++)
        {
            std::cout << energy[i] << " ";
        }
        for (int i = 0; i < 20; i++)
        {
            std::cout << accuracyarr[i] << " ";
        }
        */
    }
    return image;
}

void reset_all()
{
    cout << "reset all varaibles" << endl;
}