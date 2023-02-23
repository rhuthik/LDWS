#include <deblurring/deblur.hpp>
#include <constants.hpp>


bool deblur_image::is_variant(int deblur_selection)
{

    if(deblur_selection >= NUM_DEBLUR_VARIANTS)
    {
        return false;
    }
    else
    {
        return true;
    }

}

void deblur_image::process()
{

}