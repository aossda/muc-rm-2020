//
// Created by UNIVERSE on 2020/4/17.
//
#ifndef FINDBLUE_IMAGE_INPUT_H
#define FINDBLUE_IMAGE_INPUT_H

#include <omp.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>          //this file include resize inOpencv4
#include "opencv2/imgproc/imgproc_c.h"  //this file include IplImage in Opencv4

#define FrameCols      640
#define FrameRows      480

#endif //FINDBLUE_IMAGE_INPUT_H

class InputHead
{
public:
    virtual ~InputHead() = default;
    virtual bool init() = 0;
    virtual bool read(cv::Mat &src) = 0;
};