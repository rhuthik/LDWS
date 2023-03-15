#include <iostream>
#include <random>
#include <dirent.h>
#include <opencv2/opencv.hpp>

// Import Modules
#include <constants.hpp>
#include <shedutil/schedule.hpp>
#include <blurring/blur.hpp>
#include <thresholding/threshold.hpp>
#include <edgedetection/edge.hpp>
#include <hough/hough.hpp>

using namespace cv;
using namespace std;

// Create a Module Object
blur_image blur_image_instance;
detect_edges detect_edges_instance;
threshold_image threshold_image_instance;
detect_hough hough_instance;

void schedule::initialize()
{

    for (int module_id = 0; module_id < MODULE_COUNT; module_id++)
    {
        this->schedule_matrix.push_back(vector<Element>());
        int variant_id = 0;
        while (true)
        {
            std::chrono::microseconds worst_case_execution_energy;

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

        if (String(INITIAL_SELECTION) == "left")
        {
            variant_selection = 0;
        }
        else if (String(INITIAL_SELECTION) == "right")
        {
            variant_selection = size - 1;
        }
        else if (String(INITIAL_SELECTION) == "center-right")
        {
            if (size % 2 == 0)
                variant_selection = size / 2 + 1 - 1; //+1 for right element and -1 to adjust index as it starts from 0
            else
                variant_selection = ((size + 2 - 1) / 2) - 1;
        }
        else if (String(INITIAL_SELECTION) == "center-left")
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
    this->image_files = read_images_in_directory("../../dataset/inputs");
    this->reference_image_files = read_images_in_directory("../../dataset/inputs");

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
    std::chrono::microseconds capacity(1000000 / this->required_fps);
    this->system_capacity = capacity.count();
    // total_image_count
    this->total_image_count = static_cast<long long int>(this->image_files.size());

    // 5. Generate feasible selections

    // 6. Update selection

    // 7. Logging

    return;
}

void schedule::run()
{
    while (true)
    {
        // cout<<"150";
        // 1. Run the selection and note down the individual cycles
        cv::Mat processed_image = this->run_selection();
        // cout << "1" << endl;
        //  Breaks if there is nothing to process
        if (this->read_error_flag)
            break;
        // cout << "2" << endl;
        //  2. Compute and update Worst Case Execution Cycles and Accuracy
        this->update_worst_case_cycles_quality(processed_image);
        // cout << "3" << endl;
        //  3. Check for redundancy, update process statistics and load the image to be processed
        this->check_redundancy_statistics();
        if (this->read_error_flag)
            break;
        // cout << "4" << endl;
        //  4. Compute energy budgets
        this->compute_energy_budgets();
        // cout << "5" << endl;
        //  5. Generate feasible selections
        this->generate_feasible_selections();
        // cout << "6" << endl;
        //  6. Update selection
        this->update_selection_set();
        // cout << "7" << endl;
        //  7. Write to Logs

        // break;
    }

    return;
}

cv::Mat schedule::run_selection() // 1
{

    cv::Mat input_image = get_next_image(this->image_files, true); // image_files, is_current_image, is_reference
    if (input_image.empty())
    {
        // cout << "no image" << endl;
        this->read_error_flag = true;
        return cv::Mat();
    }
    cv::Mat blurred_image;
    cv::Mat edge_detected_image;
    cv::Mat thresholded_image;
    cv::Mat Hough_image_output;

    this->execution_costs.clear();

    // Blurring
    std::chrono::microseconds execution_cycles_blur(0);
    blur_image_instance.process(input_image, blurred_image, &execution_cycles_blur, this->selection[0].second);
    this->execution_costs.push_back(execution_cycles_blur);

    /*
    // Edge Detection
    std::chrono::microseconds execution_cycles_edge(0);
    blur_image_instance.process(blurred_image, edge_detected_image, &execution_cycles_edge, this->selection[0].second);
    this->execution_costs.push_back(execution_cycles_edge);

    // Thresholding
    std::chrono::microseconds execution_cycles_thresholding(0);
    blur_image_instance.process(edge_detected_image, thresholded_image, &execution_cycles_thresholding, this->selection[0].second);
    this->execution_costs.push_back(execution_cycles_thresholding);

    // Hough
    std::chrono::microseconds execution_cycles_hough(0);
    blur_image_instance.process(thresholded_image, Hough_image_output, &execution_cycles_hough, this->selection[0].second);
    this->execution_costs.push_back(execution_cycles_hough);

    std::chrono::microseconds initial_budget(0);
    this->time_run_selections = accumulate(this->execution_costs.begin(), this->execution_costs.end(), initial_budget);

    // Save Image

    return Hough_image_output;
    */
    std::chrono::microseconds initial_budget(0);
    this->time_run_selections = accumulate(this->execution_costs.begin(), this->execution_costs.end(), initial_budget);
    return blurred_image;
}

void schedule::update_worst_case_cycles_quality(cv::Mat output_image) // 2
{
    auto start_time = std::chrono::steady_clock::now();
    // Accuracy Calculation

    cv::Mat reference_image = get_next_image(this->reference_image_files, true);
    cv::Mat difference_image;
    cv::absdiff(output_image, output_image, difference_image);
    double mse = cv::mean(difference_image.mul(difference_image))[0];
    this->current_quality = 1 - mse;

    // Update Worst Case Cycles and accuracy
    for (int i = 0; i < this->selection_length; i++)
    {
        auto element = this->selection[i];
        if (this->schedule_matrix[element.first][element.second].energy.count() < this->execution_costs[i].count())
            this->schedule_matrix[element.first][element.second].energy = this->execution_costs[i];

        // Quality update TODO: Moving average??
        this->schedule_matrix[element.first][element.second].quality = this->current_quality;
    }

    auto end_time = std::chrono::steady_clock::now();
    this->time_update_accuracy = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    // cout << mse << endl;
    return;
}

void schedule::check_redundancy_statistics() // 3
{
    vector<std::chrono::microseconds> time_temp_vector;

    bool redundancy = true;

    while (redundancy && !this->read_error_flag)
    {

        cv::Mat current_image = get_next_image(this->image_files, true);
        cv::Mat next_image = get_next_image(this->image_files, false);
        if (next_image.empty() || current_image.empty())
        {
            this->read_error_flag = true;
            break;
        }

        cv::Mat difference_image;

        // Start the time
        auto start_time = std::chrono::steady_clock::now();

        cv::absdiff(current_image, next_image, difference_image);
        double mse = cv::mean(difference_image.mul(difference_image))[0];

        if (mse > 0)
        {
            redundancy = false;
        }
        else // update statistics like image count etc.
        {
            this->total_image_count -= 1;
        }

        if (this->total_image_count <= 0)
        {
            this->read_error_flag = true;
            break;
        }

        // End the timer and record the values
        auto end_time = std::chrono::steady_clock::now();
        time_temp_vector.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time));
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
    // cout << left_over_budget << endl;

    // Get the budget per image based on left over budget and calculate if system needs to save energy and also calculate available budget

    long long int calculated_budget_per_image = left_over_budget / this->total_image_count;

    // available extra budget for state change
    long long int calculated_budget_for_state_change = calculated_budget_per_image - computation_budget.count();
    // available system capacity for state change
    // TODO: Calculate System Capacity
    long long int system_capacity_for_processing = this->system_capacity - computation_budget.count();

    std::chrono::microseconds temp_budget(min(calculated_budget_for_state_change, system_capacity_for_processing));
    this->available_budget = temp_budget;

    if (this->available_budget.count() <= 0)
        this->save_energy = true;
    else
        this->save_energy = false;
    auto end_time = std::chrono::steady_clock::now();
    this->time_energy_budget = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    cout << this->available_budget.count() << endl;

    return;
}

void schedule::generate_feasible_selections() // 5
{
    auto start_time = std::chrono::steady_clock::now();
    this->relative_selection.clear();
    if (this->save_energy) // look towards left of the matrix
    {
        for (int i = 0; i < this->selection_length; i++)
        {
            auto current_element = this->selection[i];

            // left side
            if (current_element.second != 0)
            {
                auto left_element = this->schedule_matrix[current_element.first][current_element.second - 1];
                if (left_element.quality == 0 || (left_element.quality >= this->quality_threshold))
                {
                    if (this->compute_energy_feasibility(left_element, this->available_budget))
                    {
                        this->relative_selection.push_back(left_element);
                        if (left_element.quality == 0)
                            this->unknown_variant_flag = true;
                    }
                }
            }
            // TODO: check if there is atleast one permutation for changing.
        }
    }

    else // look towards left and right of the matrix
    {
        for (int i = 0; i < this->selection_length; i++)
        {
            auto current_element = this->selection[i];

            // left side
            if (current_element.second != 0)
            {
                auto left_element = this->schedule_matrix[current_element.first][current_element.second - 1];
                if (left_element.quality == 0 || (left_element.quality >= this->quality_threshold))
                {
                    if (this->compute_energy_feasibility(left_element, this->available_budget))
                    {
                        this->relative_selection.push_back(left_element);
                        if (left_element.quality == 0)
                            this->unknown_variant_flag = true;
                    }
                }
            }

            // right side
            if (current_element.second < static_cast<int>(this->schedule_matrix[current_element.first].size()) - 1)
            {
                auto right_element = this->schedule_matrix[current_element.first][current_element.second + 1];
                if (this->compute_energy_feasibility(right_element, this->available_budget))
                {
                    if (right_element.quality == 0 || (right_element.quality >= this->quality_threshold))
                    {
                        this->relative_selection.push_back(right_element);
                        if (right_element.quality == 0)
                            this->unknown_variant_flag = true;
                    }

                    // add else for retesting and giving another chance for band methods as there is sufficient budget
                }
            }
        }
    }
    auto end_time = std::chrono::steady_clock::now();
    this->time_generate_selections = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    this->print("relative_selection");
    this->print();
    return;
}

void schedule::update_selection_set() // 6
{
    auto start_time = std::chrono::steady_clock::now();

    // Choose the value based on a heuristic

    // 1. Current Quality is more than what is needed

    // There are no unknown variants

    // There are unknown variants

    // 2. Current Quality is less than what is needed

    if (this->update_selection_priority == "energy")
        sort(this->relative_selection.begin(), this->relative_selection.end(), compare_energy);
    else if (this->update_selection_priority == "quality")
        sort(this->relative_selection.begin(), this->relative_selection.end(), compare_quality);

    // TODO: Balanced and Random needs to fixed
    // else if (this->update_selection_priority == "energy-quality")
    //     sort(this->relative_selection.begin(), this->relative_selection.end(), compare_energy);
    // else if (this->update_selection_priority == "quality-energy")
    //     sort(this->relative_selection.begin(), this->relative_selection.end(), compare_energy);

    // else if (this->update_selection_priority == "balanced")
    //{
    //  if(this->past_selection_priority == "energy")
    //}

    int available_selections = static_cast<int>(this->relative_selection.size());
    Element selected_element;

    if (this->update_selection_mode == "max")
    {
        int temp_index = -1;
        if (this->update_selection_nature == "explorative")
        {
            std::chrono::microseconds temp(0);

            for (int i = 0; i < available_selections; i++)
            {

                if (this->relative_selection[i].energy > temp && this->relative_selection[i].quality == 0)
                {
                    temp = this->relative_selection[i].energy;
                    temp_index = i;
                }
            }
            if (temp_index != -1)
            {
                selected_element = this->relative_selection[temp_index];
            }
        }
        if (temp_index == -1)
        {
            selected_element = this->relative_selection[available_selections - 1];
        }
    }
    else if (this->update_selection_mode == "min")
    {
        int temp_index = -1;
        if (this->update_selection_nature == "explorative")
        {
            std::chrono::microseconds temp(9999999999);

            for (int i = 0; i < available_selections; i++)
            {

                if (this->relative_selection[i].energy < temp && this->relative_selection[i].quality == 0)
                {
                    temp = this->relative_selection[i].energy;
                    temp_index = i;
                }
            }
            if (temp_index != -1)
            {
                selected_element = this->relative_selection[temp_index];
            }
        }
        if (temp_index == -1)
        {
            selected_element = this->relative_selection[0];
        }
    }
    // TODO:  Other modes of operation
    else if (this->update_selection_mode == "center-left")
    {
        if (this->update_selection_nature == "explorative")
        {
        }
        else
        {
        }
    }
    else if (this->update_selection_mode == "center-right")
    {
        if (this->update_selection_nature == "explorative")
        {
        }
        else
        {
        }
    }

    else if (this->update_selection_mode == "random")
    {
        if (this->update_selection_nature == "explorative")
        {
        }
        else
        {
        }
    }

    this->selection[selected_element.module_id] = make_pair(selected_element.module_id, selected_element.variant_id);

    for (int i = 0; i < 4; i++)
    {
        std::cout << "Index: " << i << ", Value: (" << this->selection[i].first << ", " << this->selection[i].second << ")" << std::endl;
    }
    this->unknown_variant_flag = false;
    auto end_time = std::chrono::steady_clock::now();
    this->time_update_selections = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return;
}

// Optional Functions

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
        /*
        for (int i = 0; i < static_cast<int>(this->left_relative_selection.size()); i++)
        {
            //cout << "(";
            //cout << this->left_relative_selection[i].module_id << ", ";
            //cout << this->left_relative_selection[i].variant_id << ", ";
            //cout << this->left_relative_selection[i].energy.count() << ", ";
            //cout << this->left_relative_selection[i].quality << "), ";
        }
        //cout << endl;
        for (int i = 0; i < static_cast<int>(this->right_relative_selection.size()); i++)
        {
            //cout << "(";
            //cout << this->right_relative_selection[i].module_id << ", ";
            //cout << this->right_relative_selection[i].variant_id << ", ";
            //cout << this->right_relative_selection[i].energy.count() << ", ";
            //cout << this->right_relative_selection[i].quality << "), ";
        }
        //cout << endl;
        */
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

void schedule::log()
{
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
        std::cerr << "Error opening directory " << directory << std::endl;
        return image_files;
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
    for (auto element : image_files)
    {
        cout << element << " " << endl;
    }
    cout << endl;
    return image_files;
}

// Read Images

cv::Mat get_next_image(std::vector<std::string> image_files, bool is_current_image)
{

    static int image_index = 0;
    cv::Mat image;
    if (!is_current_image)
        image_index++;
    if (image_index < static_cast<int>(image_files.size()))
    {
        cv::Mat image = cv::imread(image_files[image_index]);

        if (image.empty())
        {
            // cout << "Could not read image" << std::endl;
        }
        return image;
    }
    else
    {
        // cout << "All Images have been processed" << endl;
    }
    return image;
}