#include <iostream>
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

   // Run the System
   scheduler.run();

   return 0;
}
