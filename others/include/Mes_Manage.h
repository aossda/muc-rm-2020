#pragma once
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include<stdio.h>
#include<iostream>


#include"../../armor/include/ArmorClass.h"
#include"../../armor/include/Num_Select.h"
#include"Mes_Manage.h"

#define FrameCols			640
#define FrameRows			480
#define CameraYawAngle		60
#define CameraPitchAngle	60

using namespace std;
using namespace cv;

class Mes_Manage
{
public:

    float Ytan = tan(CameraYawAngle * PI / 180), Ptan = tan(CameraPitchAngle * PI / 180);

    const char* Dev = "/dev/ttyTHS2";                  //UART3
	int fd;
	int countOut = 0, fcount = 0, count_send_times = 0, success = 0;
	int afy = 2;   //aim offset x
	int afx = 1;    //aim offset y
	char data[8]; //fire,yawAngle,PitchAngle,check

	float Yaw = 0, Pitch = 0;

	bool debug = 0;
	bool inAimArea = false, ifSaveVideo = false;

	cv::Mat videoFrame;
	VideoWriter video;

	Mes_Manage();

	void Send_Message();
	void Set_Miss();
	void saveVideo(cv::Mat frame);
    void Angle_Predict(Armor& target, int &lostNum);
	void set_Mes();
    void AngleSolver(Armor armor, float& YawAngle, float& PitchAngle);

};