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
  schedule scheduler;

  // Initialize the system
  scheduler.initialize();
  
  cout << "Initialization Complete" << endl;

  scheduler.run();

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
