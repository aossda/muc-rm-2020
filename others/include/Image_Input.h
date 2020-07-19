//
// Created by UNIVERSE on 2020/4/17.
//
#include<omp.h>
#include<opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include<iostream>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>          //whitch file include resize inOpencv4
#include "opencv2/imgproc/imgproc_c.h"  //which file include IplImage in Opencv4


#ifndef FINDBLUE_IMAGE_INPUT_H
#define FINDBLUE_IMAGE_INPUT_H

#endif //FINDBLUE_IMAGE_INPUT_H

class InputHead
{
public:
    virtual ~InputHead() = default;
    virtual bool init() = 0;
    virtual bool read(cv::Mat &src) = 0;
};