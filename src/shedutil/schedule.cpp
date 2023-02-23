#include <shedutil/schedule.hpp>
#include <deblurring/deblur.hpp>

void schedule::initialize()
{
    deblur_image deblur_image_instance;
    int i = 0;
    while(true)
    {
        if (deblur_image_instance.is_variant(i))
        {
            cout << "i: " << i << endl;
            i++;
        }
        else
        {
            break;
        }
    }
    cout << "initializing" << endl;
    return;
}

void schedule::run()
{
    return;
}