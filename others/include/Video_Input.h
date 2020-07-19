//
// Created by UNIVERSE on 2020/4/20.
//
#include "Image_Input.h"
#ifndef FINDBLUE_VIDEO_INPUT_H
#define FINDBLUE_VIDEO_INPUT_H
#define Normal_Cam		1
#define Video_Mode		2
#define Picture         3
#define PI  3.1416

#define INPUT_VIDEO		"..//video//1.mp4"

#endif //FINDBLUE_VIDEO_INPUT_H

class VideoInput :public InputHead {
public:
    VideoInput(std::string &input);
    bool init() final;
    bool read(cv::Mat& src) final;
private:
    cv::Mat tmp;
    cv::VideoCapture origin;
    int camera_mode;

};