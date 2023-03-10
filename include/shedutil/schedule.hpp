#ifndef INCLUDE_SCHEDUTIL_HPP
#define INCLUDE_SCHEDUTIL_HPP

#include <iostream>
#include <vector>
#include <blurring/blur.hpp>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


struct Element {
    int module_id;
    int variant_id;
    std::chrono::microseconds energy;
    float quality;
};


class schedule
{

public:

   vector<vector<Element>> schedule_matrix;
   vector<int> selection;
   vector<Element> sorted_selection;

   void initialize();
   void run();
   void print(String print_type = "matrix"); 
};

bool compare_elements(const Element& e1, const Element& e2);



#endif 