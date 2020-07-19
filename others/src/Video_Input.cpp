//
// Created by UNIVERSE on 2020/4/20.
//

#include "../include/Video_Input.h"

VideoInput::VideoInput(std::string &input)
{
    origin.open(input);
    if (origin.isOpened())
    {
        printf("Input Video is opened successfully\n");
    }
    else{
        printf("Input Video failed to open!\n");
        init();
    }
}
bool VideoInput::init()
{
    origin.open(0);
    if (origin.isOpened())
    {
        printf("Old Video is opened successfully\n");
        return true;
    }
    else
        printf(" Old Video failed to open!\n");

    origin.open(INPUT_VIDEO);
    if (origin.isOpened())
    {
        printf("Usb Camera is opened successfully\n");
        return true;
    }
    else
    {
        printf("Usb Camera failed to open!\n");
        return false;
    }
}

bool VideoInput::read(cv::Mat& input)
{
    origin >> tmp;
    cv::resize(tmp, input, cv::Size(640, 480));
    return true;
}

