#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>

#include <ldws.hpp>
#include <constants.hpp>
#include <shedutil/schedule.hpp>

using namespace cv;
using namespace std;

int main()
{
  // Create a scheduler object
  //(redundancy_threshold, quality_threshold, required_fps, total_energy_budget, update_selection_nature,
  // update_selection_priority, update_selection_mode, initial_selection_mode)

  // Declare Entities
  vector<double> r_threshold;
  vector<float> q_threshold;
  vector<long long int> fps;
  vector<long long int> energy_budget;
  vector<String> nature;
  vector<String> priority;
  vector<String> mode;
  vector<String> initial_selection;

  // Initialize them
  // r and q threshold
  for (int temp_i = 0; temp_i <= 100; temp_i += 5)
  {
    r_threshold.push_back(35);
    q_threshold.push_back(20);
    break;
  }
  // fps
  // fps.push_back(15);
  fps.push_back(30);
  // fps.push_back(45);
  // fps.push_back(60);
  //  energy_budget
  energy_budget.push_back(99999999);
  // nature
  nature.push_back("explorative");
  // nature.push_back("non-explorative");
  //  priority
  // priority.push_back("quality");
  priority.push_back("energy");
  // mode
  // mode.push_back("max");
  mode.push_back("min");
  // intial selection
  initial_selection.push_back("left");
  // initial_selection.push_back("right");
  // initial_selection.push_back("center_left");
  // initial_selection.push_back("center_right");
  // initial_selection.push_back("random");

  // Use them
  for (int redundancy_value = 0; redundancy_value < static_cast<int>(r_threshold.size()); redundancy_value++)
  {
    for (int quality_value = 0; quality_value < static_cast<int>(q_threshold.size()); quality_value++)
    {
      for (int fps_value = 0; fps_value < static_cast<int>(fps.size()); fps_value++)
      {
        for (int energy_value = 0; energy_value < static_cast<int>(energy_budget.size()); energy_value++)
        {
          for (int nature_value = 0; nature_value < static_cast<int>(nature.size()); nature_value++)
          {
            for (int priority_value = 0; priority_value < static_cast<int>(priority.size()); priority_value++)
            {
              for (int mode_value = 0; mode_value < static_cast<int>(mode.size()); mode_value++)
              {
                for (int initial_value = 0; initial_value < static_cast<int>(initial_selection.size()); initial_value++)
                {
                  schedule scheduler(r_threshold[redundancy_value], q_threshold[quality_value],
                                     fps[fps_value], energy_budget[energy_value], nature[nature_value],
                                     priority[priority_value], mode[mode_value], initial_selection[initial_value]);

                  // Initialize the system
                  scheduler.initialize();

                  scheduler.run();
                }
              }
            }
          }
        }
      }
    }
  }
  // schedule scheduler;

  // Initialize the system
  // scheduler.initialize();

  // scheduler.run();

  return 0;
}

// Run the System
// long m;
// #pragma omp parallel
// {
//  if (omp_get_thread_num() == 0)
//     m = omp_get_num_threads();
// }
// #pragma omp parallel num_threads(m)
//   {

// }
