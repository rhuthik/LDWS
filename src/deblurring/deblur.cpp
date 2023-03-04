#include <constants.hpp>
#include <deblurring/deblur.hpp>
#include <deblurring/deblur_box.hpp>
#include <deblurring/deblur_median.hpp>
#include <deblurring/deblur_gaussian.hpp>
#include <deblurring/deblur_histogram.hpp>


bool deblur_image::is_variant(int deblur_selection)
{
    //start from  zero
    if(deblur_selection >= NUM_DEBLUR_VARIANTS)
    {
        return false;
    }
    else
    {
        return true;
    }

}

//void deblur_image::get_tick_count(cv::Mat gray,cv::Mat& blur, int deblur_selection_id)
//{

//}

//void deblur_image::process(cv::Mat gray,cv::Mat& blur, int deblur_selection_id)
//{
 
//}
