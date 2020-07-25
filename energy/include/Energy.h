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

using namespace std;
using namespace cv;
class Energy {
private:
public:
    enum {KALMAN, MIN2X};
    bool debug = 1;
    cv::Mat image, binary;

    //Kalman Algorithm
    int stateNum = 4;
    int measureNum = 2;
    KalmanFilter KF;
    Mat processNoise;
    Mat measurement = Mat::zeros(measureNum, 1, CV_32F);

    RotatedRect rrect;//target
    cv::Point2d target;

    void KalmanInit();
    cv::Point2d KalmanFind(cv::Mat &input);
    
    vector<cv::Point2d> points;
    int LeastSquaresCircleFitting(vector<cv::Point2d>& m_Points, cv::Point2d& Centroid, double& dRadius); //拟合圆函数
    cv::Point2d Min2XFind(cv::Mat &input);
    
    Energy();
};


#endif //FINDBLUE_ENERGY_H
