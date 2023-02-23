#ifndef INCLUDE_SCHEDUTIL_HPP
#define INCLUDE_SCHEDUTIL_HPP

#include <iostream>
#include <vector>
#include <deblurring/deblur.hpp>

using namespace std;

class schedule
{

    pair<float, float> variant = make_pair(0,0);  // worst_case_time, relative_quality_change
    vector<pair<float, float>> module;
    vector<vector<pair<float, float>>> schedule_matrix;

public:


   void initialize();
   void run();


};


#endif 