#ifndef INCLUDE_LDWS_HPP
#define INCLUDE_LDWS_HPP

#include <cstdio>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace program
{
    inline void foo()
    {
        printf("program::foo()\n");
        cout << "OpenCV version : " << CV_VERSION << endl;
        cout << "Major version : " << CV_MAJOR_VERSION << endl;
        cout << "Minor version : " << CV_MINOR_VERSION << endl;
        cout << "Subminor version : " << CV_SUBMINOR_VERSION << endl;
    }
}

#endif
