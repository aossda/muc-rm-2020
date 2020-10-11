//
// Created by UNIVERSE on 2020/4/20.
//

#include "../include/Video_Input.h"

VideoInput::VideoInput()
{
    init();
}

VideoInput::VideoInput(const std::string &input)
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

VideoInput::~VideoInput()
{
    std::cout << "VideoInput Released!\n";
}

bool VideoInput::init()
{
    std::cout << "Try openning pre-defined video source \"" << INPUT_VIDEO << "\"" << std::endl;
    origin.open(INPUT_VIDEO);
    if (origin.isOpened())
    {
        printf("successfully opened \"%s\"\n", INPUT_VIDEO);
        return true;
    }
    else
        printf("Failed to open \"%s\"\n", INPUT_VIDEO);
    
    std::cout << "Try openning usb camera source" << std::endl;
    origin.open(0);
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
    if(tmp.empty()) {
        std::cout << "Cannot load frame from video." << std::endl;
        origin.release();
        return false;
    }
    else {
        cv::resize(tmp, input, cv::Size(FrameCols, FrameRows));
        return true;
    }
}
