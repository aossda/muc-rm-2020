#pragma once
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include<opencv2/imgproc/types_c.h> //CV_XXX_XXX in this file
#include<stdio.h>
#include<iostream>

//#define FIND_RED            1
//#define FIND_BLUE           2

#define FrameCols			640
#define FrameRows			480

#define g_nContrastValue	40//100 ����
#define g_nBrightValue		20//80
#define KernelX				3
#define KernelY				3
#define ContoursSizeMin		15  //at least 6

#define LightBarProMin		1
#define LightBarProMax		10

// Binary and Thres
#define BinaryThresBlue			150   //200		2 ֵ������
#define BinaryThresRed          120
#define redThres			30     //70;	��ɫ������ȡ
#define blueThres			70
using namespace std;
using namespace cv;

class ImgManage
{
public:
	cv::Mat frame0, frame, gray,res_norm, xxx;;
	Mat DstImg;//		����������ͼ
	bool debug = 0;
	cv::Mat RgbChannels[3];

	//------------���������--------------//
	cv::Mat thresDilate = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 4));
	cv::Mat thresErode = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
	cv::Mat colorDilate = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 6));
	cv::Mat colorErode = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
	

	Mat dstDilate = getStructuringElement(MORPH_RECT, Size(3, 4));
	Mat dstErode = getStructuringElement(MORPH_RECT, Size(2, 1));

	cv::Size imgSize = cv::Size(640, 480);

	vector<vector<Point> > contours;
	vector<vector<Point> > contours2;
	vector<vector<Point> > fgcontours;

	vector<RotatedRect> Lightbar;

	/*VideoCapture c("/home/nvidia/opencv/findblue/xuanzhuan.avi");
	if(!c.isOpened())
	{
		cout<<"open video failed"<<endl;
		return 0;
	}*/
	//-----------------------------------------loop----------------------------------------//
//    int cc=0;
//    int aa=0;
	enum {FIND_BLUE, FIND_RED};

    void Pre_Manage(cv::Mat &input, int mode);
	//------------------------------get image----------------------------//
	void lightExam(vector<vector<Point> > fgcontours, vector<vector<Point> > bluecontoursl, Mat* dst, Mat fgImg);
	void LightBarExtract(Rect LightBarRect, Mat BlueImg, Mat Binaryimg, Mat& DstImg);
    bool isInside(Rect &rect1, Rect &rect2);
	void input_operation(cv::Mat& input, cv::Mat& BinaryImg, cv::Mat& colorImg, int &mode);
	cv::Mat get_dstimg(cv::Mat& input, cv::Mat& BinaryImg, cv::Mat& colorImg);
	void get_Lightbar();
	void ContrastAndBright(Mat& img, Mat& g_dstImage);

};

//void Threshold(int, void*);
//void  ExtractRed(int, void*);