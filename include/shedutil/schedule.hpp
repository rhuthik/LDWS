#ifndef INCLUDE_SCHEDUTIL_HPP
#define INCLUDE_SCHEDUTIL_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <blurring/blur.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

struct Element
{
    int module_id;
    int variant_id;
    std::chrono::microseconds energy;
    float quality;
};

class schedule
{

public:
    schedule(double redundancy_threshold, float quality_threshold,
             long long int required_fps, long long int total_energy_budget,
             String update_selection_nature, String update_selection_priority,
             String update_selection_mode, String initial_selection_mode);
    // initialization
    vector<vector<Element>> schedule_matrix;
    vector<pair<int, int>> selection;
    vector<Element> relative_selection;
    String initial_selection_mode = String(INITIAL_SELECTION);

    int selection_length = 0;

    // Processing requirements and other elements
    long long int image_count = 0;
    long long int total_image_count = 0;
    long long int system_capacity;

    // Configurations
    double redundancy_threshold = 42;
    float quality_threshold = 15;
    long long int required_fps = 30;
    long long int total_energy_budget = 9999999;

    // Run time Storage
    vector<float> accuracy_per_image;
    vector<int> energy_per_image;

    void initialize();
    int max_module_length = 0;
    // Optional
    vector<Element> left_relative_selection;
    vector<Element> right_relative_selection;

    // Run Time Algorithm
    void run();

    // Run the Selection // 1
    bool read_error_flag = false;

    std::vector<std::string> image_files;
    std::vector<std::string> reference_image_files;
    cv::Mat run_selection();

    // Compute and update Worst case Execution Cycles and Accuracy // 2
    float current_quality = 0;
    void update_worst_case_cycles_quality(cv::Mat output_image);

    // Check for redundancy, update process statistics and load the image to be processed // 3
    void check_redundancy_statistics();

    // Compute energy budgets // 4
    std::chrono::microseconds time_run_selections;      // 1
    std::chrono::microseconds time_update_accuracy;     // 2
    std::chrono::microseconds time_update_redundancy;   // 3
    std::chrono::microseconds time_energy_budget;       // 4
    std::chrono::microseconds time_generate_selections; // 5
    std::chrono::microseconds time_update_selections;   // 6

    vector<std::chrono::microseconds> execution_costs;
    std::chrono::microseconds current_budget_per_frame;
    std::chrono::microseconds available_budget_per_frame;

    void compute_energy_budgets();

    // Generate feasible selections // 5
    bool save_energy = false;
    std::chrono::microseconds available_budget;
    void generate_feasible_selections();

    // Optional
    int get_actual_position(int module_id, int row_id);
    void get_relative_selections();
    void generate_feasibility_set(std::chrono::microseconds available_budget, float quality_threshold);

    // Update Selection // 6
    bool unknown_variant_flag = false;
    String update_selection_nature = "explorative"; // can be explorative and non-explorative
    String update_selection_priority = "quality";   // can be energy, quality, energy-quality, quality-energy, balanced   1. n-e, energy, min 2. n-e, quality, min  3. best mode 4. worst mode
    String update_selection_mode = "max";           // max, min, center-left, center-right, random
    void update_selection_set();

    // Log Components
    std::ofstream txt_file;
    std::ofstream data_file;
    int selected_module_id = -1;
    int selected_variant_id = -1;
    String dataset = "../../dataset/results/files/kitti";

    // Utility Functions
    void log(String state = "run");
    void print(String print_type = "matrix");
    void reconfigure_inputs(std::chrono::microseconds execution_time);
    bool compute_energy_feasibility(Element e, std::chrono::microseconds available_budget);
};

bool compare_energy(const Element &e1, const Element &e2);
bool compare_quality(const Element &e1, const Element &e2);
vector<std::string> read_images_in_directory(const std::string directory);
cv::Mat get_next_image(std::vector<std::string> image_files, bool is_current_image, String purpose);
void reset_all();

#endif