#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>

// Import Modules
#include <constants.hpp>
#include <shedutil/schedule.hpp>
#include <blurring/blur.hpp>
#include <thresholding/threshold.hpp>
#include <edgedetection/edge.hpp>


using namespace cv;
using namespace std;

// Create a Module Object 
blur_image blur_image_instance;
detect_edges detect_edges_instance;
threshold_image threshold_image_instance;

void schedule::initialize()
{
    
    for(int module_id=0; module_id<MODULE_COUNT; module_id++)
    {
        this->schedule_matrix.push_back(vector<Element>());
        int variant_id = 0;
        while(true)
        {
            std::chrono::microseconds worst_case_execution_energy;

            if(module_id==0 && blur_image_instance.is_variant(variant_id)) // Deblurring
                worst_case_execution_energy = blur_image_instance.get_tick_count(variant_id);

            else if(module_id==1 && blur_image_instance.is_variant(variant_id)) // Thresholding
                worst_case_execution_energy = blur_image_instance.get_tick_count(variant_id);

            else if(module_id==2 && detect_edges_instance.is_variant(variant_id)) // Edge Detection
                worst_case_execution_energy = detect_edges_instance.get_tick_count(variant_id);

            else if(module_id==3 && detect_edges_instance.is_variant(variant_id)) // Line Detection (Hough)
                worst_case_execution_energy = detect_edges_instance.get_tick_count(variant_id);

            else
                break;

            // Run each variant mutiple times and push back the tuple with initial values
            cout << "Module ID: " << module_id << " Variant ID: " << variant_id << " with worst case execution time: " << worst_case_execution_energy.count() <<" us" <<endl; 
            this->schedule_matrix[module_id].push_back(Element{module_id, variant_id, worst_case_execution_energy, 0.0});
            variant_id++;
        }
    }
        // before Sorting the values
        this->print();

        //Sort the techniques across each row
        for (std::vector<int>::size_type i = 0; i < this->schedule_matrix.size(); i++)
            sort(this->schedule_matrix[i].begin(), this->schedule_matrix[i].end(), compare_elements);
        
        // Print the sorted elements of the matrix
        this->print();

        // Initial Selection
        // Three variants Right most, leftmost, center, random
        for (std::vector<int>::size_type i = 0; i < this->schedule_matrix.size(); i++)
        {
            int selection = 0;
            int size = static_cast<int>(this->schedule_matrix[static_cast<int>(i)].size());
            
            if(String(INITIAL_SELECTION)  == "left")
            {
                selection = 0;
            }
            else if(String(INITIAL_SELECTION) == "right")
            {
                selection = size - 1;
            }
            else if(String(INITIAL_SELECTION)  == "center-right")
            {
                if(size%2 == 0)
                    selection = size/2 + 1 -1;  //+1 for right element and -1 to adjust index as it starts from 0
                else
                    selection = ((size+2-1)/2) - 1;
            }
            else if(String(INITIAL_SELECTION)  == "center-left")
            {
                if(size%2 == 0)
                    selection = size/2 - 1;  //+1 for right element and -1 to adjust index as it starts from 0
                else
                    selection = ((size+2-1)/2) - 1;
            }
            else
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::vector<int> choices {0, size-1, ((size+2-1)/2) - 1};
                std::uniform_int_distribution<> dist(0, 2);
                selection = choices[dist(gen)];

            }
            this->selection.push_back(selection);
            this->sorted_selection.push_back(this->schedule_matrix[static_cast<int>(i)][selection]);
            cout << selection;
        }
        this->print("sorted_selection");
        //sort the selected permutations
        sort(this->sorted_selection.begin(), this->sorted_selection.end(), compare_elements);
        this->print("sorted_selection");
    return;
}

void schedule::run()
{
    return;
}

void schedule::get_relative_selections(bool enough_energy, std::chrono::microseconds max_available_energy_budget)
{
    return;
}

void schedule::print(String print_type )
{                                       
    cout << endl;

    if(print_type == "sorted_selection")
    {
        for (std::vector<int>::size_type i = 0; i < this->sorted_selection.size(); i++)
        {
            cout << "(";
            cout << this->sorted_selection[i].module_id << ", ";
            cout << this->sorted_selection[i].variant_id << ", ";
            cout << this->sorted_selection[i].energy.count() << ", ";
            cout << this->sorted_selection[i].quality << "), ";
        }

    }

    else if(print_type == "matrix")
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


bool compare_elements(const Element& e1, const Element& e2) 
{
    return e1.energy < e2.energy;
}