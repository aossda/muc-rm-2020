#pragma once
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>

#include<iostream>

#include"ArmorClass.h"
#include"Num_Select.h"
#include <opencv2/tracking.hpp>

//#define USE_MULTI_THREAD
#include"eco.hpp"
//#include"Mes_Manage.h"

using namespace std;
using namespace cv;

#define ArmorHeightMin		12
#define LightBarHeightMin	8
#define FrameCols			640
#define FrameRows			480

#define AngleThres			10
#define WidthThres			250
#define ArmorRatiox			4
#define ArmorRatioy			20
#define HeightThres			170
#define aWeight				1
#define bWeight				10
#define cWeight				10
#define aimAreaWidth		0.07
#define aimAreaHeight		0.07

#define heightCoeff			1.5

class ArmorDetect
{
public:
	
	enum {DEFAULT, TRACKING, INIT_TRACKING, ARMORFIND};
	int status = ARMORFIND;
	ostringstream oss;
	float ArmorProMin = 0.7, ArmorProMax = 4; //frame saved count
	bool debug = 0, flag_get = false;
    int lostNum = 0;//lost page
	int antiShake = 100;
	int shakeThres = 5;

	uint choice = 0, choice1 = 0, choice2 = 0;

	Size imgSize = Size(640, 480);

	float lastdata = 10;
	int lastfire = 10;

	Point2f pt[4];

	Num_Select SVM;
    Armor lastArmor;
    float lastshoot_center_x = 0;
    float lastshoot_center_y = 0;

	cv::Mat frame0;
	//----------------------------------get score-------------------------------------

	//int width = 480;
	//int length = 640;
	//int max_area = width * length;

	//scores preset for different armors: 8>1=6>3=4=5>2
	int preset_scores[9] = { 0,8,6,7,7,7,8,6,9 };


    Armor target ;
	//--------------------------------function---------------------------------------
	//Mes_Manage Send;

	void Armor_Contours(vector<RotatedRect>& LightBar, cv::Mat & frame);
	
	float AngleConvert(float RowAngle);    //convert to poleAngle

	float AngleConvert2(float poleAngle);   //convert poleAngle to rotatedRect angle


	void DrawArmor(Mat& frame, Point2f pt[]);

	vector<Armor> Detect(vector<RotatedRect> &LightBar, vector<Armor>&armor);

	void GetScore(vector<Armor> &armor);
	void ArmorGrading(vector<Armor>& armors);//2020

	void Choice(vector<Armor>& armor);
	void final_choice(vector<Armor>& armor);

//Traking
	void tracking(Mat& frame);
	eco::ECO * ecotracker = NULL;
	eco::EcoParameters parameters;
	bool end_tracking();
	//Rect2d bbox;
	Rect2f bbox;
	int frame_cols = 640;
	int frame_rows = 480;
	bool generate_bbox(Armor&, Rect2f &bbox);
	//bool if_tracking = false;
	//bool if_init = false;
	Armor the_target;
	Ptr<Tracker> tracker;
	//int tot_fail;
	//int tot_tracks;
	//int fails_in_a_raw;
	int regular_check;
	Point2d last_center;
	bool previous_exist = false;
	int last_tracked_frames = 16;

	ArmorDetect(){};
};