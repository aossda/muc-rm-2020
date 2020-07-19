//
// Created by UNIVERSE on 2020/5/13.
//
#include <iostream>
#include "opencv2/opencv.hpp"
#include<opencv2/imgproc/types_c.h> //CV_XXX_XXX in this file
#include <math.h>

#ifndef FINDBLUE_ENERGY_H
#define FINDBLUE_ENERGY_H

#define FrameRows       480
#define FrameCols       640

#define stateNum        4
#define measureNum      2

using namespace std;
using namespace cv;
class Energy {
private:

    bool debug = 1;
    cv::Mat image, binary;
    vector<cv::Point2d> points;

    KalmanFilter KF;
    Mat processNoise;
    Mat measurement = Mat::zeros(measureNum, 1, CV_32F);

    RotatedRect rrect;//target
    cv::Point2d target;
public:
    Energy();
    void start(cv::Mat &input);
    cv::Point2d get_target_min2X();
    cv::Point2d get_target_Kalman();
    int LeastSquaresCircleFitting(vector<cv::Point2d>& m_Points, cv::Point2d& Centroid, double& dRadius);
};


#endif //FINDBLUE_ENERGY_H
