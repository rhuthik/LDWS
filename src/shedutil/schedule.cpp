#include <iostream>
#include <opencv2/opencv.hpp>

// Import Modules
#include <constants.hpp>
#include <shedutil/schedule.hpp>
#include <blurring/blur.hpp>
#include <edgedetection/edge.hpp>


using namespace cv;
using namespace std;

// Create a Module Object 
blur_image blur_image_instance;
detect_edges detect_edges_instance;

void schedule::initialize()
{
    
    for(int module_id=0; module_id<MODULE_COUNT; module_id++)
    {
        int variant_id = 0;
        while(true)
        {
            if(module_id==0) // Deblurring
            {
                if (blur_image_instance.is_variant(variant_id))
                {
                    std::chrono::microseconds tick_count = blur_image_instance.get_tick_count(variant_id);
                    cout << "Module ID: " << module_id << " Variant ID: " << variant_id << " with " << tick_count.count() <<" ticks" <<endl; // Run each variant with mutiple times and push Back the tuple with initial values
                }
                else
                {
                    break;
                }

            }
            else if(module_id==1) // Thresholding
            {
                if (blur_image_instance.is_variant(variant_id))
                {
                    std::chrono::microseconds tick_count = blur_image_instance.get_tick_count(variant_id);
                    cout << "Module ID: " << module_id << " Variant ID: " << variant_id << " with " << tick_count.count() <<" ticks" <<endl; // Run each variant with mutiple times and push Back the tuple with initial values
                }
                else
                {
                    break;
                }
            }
            else if(module_id==2) // Edge Detection
            {
                if (detect_edges_instance.is_variant(variant_id))
                {
                    std::chrono::microseconds tick_count = detect_edges_instance.get_tick_count(variant_id);
                    cout << "Module ID: " << module_id << " Variant ID: " << variant_id << " with " << tick_count.count() <<" ticks" <<endl; // Run each variant with mutiple times and push Back the tuple with initial values
                }
                else
                {
                    break;
                }
            }
            else if(module_id==3) // Line Detection (Hough)
            {
                if (detect_edges_instance.is_variant(variant_id))
                {
                    std::chrono::microseconds tick_count = detect_edges_instance.get_tick_count(variant_id);
                    cout << "Module ID: " << module_id << " Variant ID: " << variant_id << " with " << tick_count.count() <<" ticks" <<endl; // Run each variant with mutiple times and push Back the tuple with initial values
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }

            variant_id++;
        }
    }
    return;
}

void schedule::run()
{
    return;
}