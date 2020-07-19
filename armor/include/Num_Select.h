#pragma once
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include<opencv2/imgproc/types_c.h>

#include<iostream>

#include"ArmorClass.h"

using namespace cv;
using namespace std;

#define imgWidth		40
#define imgHeight		40
#define SVM_PATH		"..//xml//2020svm_v2_123458.xml"//"/home/nvidia/opencv/findred/svm.xml"
#define SAVE_PATH		"..//svmimg//"//"/home/nvidia/opencv/findred/image/"

class Num_Select
{
public:

    bool debug = 1;

	int countOut = 200;
	int csvm = 120;
	int svmcount = 0;
	ostringstream oss;
	int imgNum = 0;
	Mat aImage;

	//------------SVM-----------//
	Ptr<ml::SVM> svm = Algorithm::load<ml::SVM>(SVM_PATH);
	HOGDescriptor* hog = new HOGDescriptor(Size(40, 40), Size(20, 20), Size(10, 10), Size(10, 10), 9);
	//HOGDescriptor* hog = new HOGDescriptor(Size(9, 9), Size(6, 6), Size(3, 3), Size(3, 3), 9);
	//HOGDescriptor* hog = new HOGDescriptor(Size(6, 6), Size(6, 6), Size(3, 3), Size(3, 3), 9);
	vector<float> descriptors;

	//Armor armor;
    bool use_SVM(cv::Mat& frame, Armor& armor);

	//-------------------Euclidean Distance----------------//ŷʽ����
	Mat  tem_image;
	Mat tmp[8];
	string filename = "..//img//";

	bool Euclidean_distance(Mat& src, Armor& armor);

    //--------------------all used-------------------------//
    Num_Select();
    bool depart_img(cv::Mat& frame, Armor& armor);
	void DrawPoint(cv::Mat frame, cv::Point2f* pt);
    void Save_Picture(cv::Mat aImage);
};