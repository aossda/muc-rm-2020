#include<opencv2/opencv.hpp>
#include<omp.h>
#include<opencv2/core.hpp>
#include<iostream>


#include"others/include/Mes_Manage.h"
#include"armor/include/Armor_Detect.h"
#include"armor/include/Img_Manage.h"
#include "energy/include/Energy.h"


#ifdef USE_INDUSTRY_CAMERA
    #include"Camera_Input.h"
#else
    #include"others/include/Video_Input.h"
#endif

#include "others/include/config.h"             //cmake define USE_INDUSTRY_CAMERA in it

using namespace std;
using namespace cv;

bool cameraReady = false;
Mes_Manage Send;

bool debug = false;
bool ifSaveVideo = true;
int scount = 0;

//--------------------------version of single thread------------------------------------//

void time(double& t)
{
	cout << "<time> :" << 1000 * ((double)cv::getTickCount() - t) / cv::getTickFrequency() << " ms\n\n";
}

/*void releaseInputHead(InputHead* & ptr)
{
    if (ptr) {
        delete (ptr);
        ptr = nullptr;
    }
}*/

void stamp_info_on_frame(Mat& frame, double gap, bool if_tracking = false)
{
    static Scalar color_orange(0,139,243);
    static Scalar color_green(50,170,50);
    static Scalar color_red(73,53,254);
    string time_str = to_string(1000 * (gap) / cv::getTickFrequency()) + " ms";
    int fps = getTickFrequency() / gap;
    
    Scalar * text_color;
    if (fps < 50) text_color = &color_red;
    else if (fps < 70) text_color = &color_orange;
    else text_color = &color_green;

    putText(frame, time_str, Point(10,20), FONT_HERSHEY_SIMPLEX, 0.75, *text_color, 2);
    putText(frame, "FPS : " + to_string(int(fps)), Point(10, 50), FONT_HERSHEY_SIMPLEX, 0.75, *text_color, 2);
    if (if_tracking) putText(frame, "Tracking...", Point(10, 80), FONT_HERSHEY_SIMPLEX, 0.75, *text_color, 2);
}

int main()
{
	Ptr<InputHead> camera;
	ArmorDetect armorfind;
	ImgManage pre_img;
	Energy energy;
	cv::Mat datax;
    
    bool xxx = true;
	long int last_count = 0;//计数最后一次图像数

#ifdef USE_INDUSTRY_CAMERA
	camera = new CameraInput();
#else
    camera = new VideoInput("..//video//1.mp4");
    //camera = new VideoInput("..//video//energy1.mp4");
#endif
    VideoWriter * outputsample = nullptr;
    bool save_sample = false;
    if(save_sample)
    {
        outputsample = new VideoWriter("..//video//sample.mp4",CV_FOURCC('A','V','C','1'),30,Size(640,480));
    }

    while (xxx)
    {
        //int working_mode = ImgManage::FIND_RED;
        double t = (double)cv::getTickCount();

        scount++;
        bool ok = camera->read(datax);
        //datax = imread("..//video//2.png");
        //resize(datax,datax,Size(640,480));
        if (!ok) {
            //releaseInputHead(camera);
            break;
        }

        imshow("in", datax); waitKey(1);

        if (scount <= 250)//		跳过前xxx帧图像
            continue;

        int flag_scount = scount;


        if (last_count != flag_scount)//	线程结束标识符
            last_count = flag_scount;
        /*else
        {
            cout << "Process Thread finished!\n";
            break;
        }*/
        bool if_armor_find = true;
        waitKey(1);//  add jundge code in this place
        if(if_armor_find)//armor mode
        {
            if (armorfind.status == ArmorDetect::TRACKING) {
                armorfind.tracking(datax);
            } 
            else {
                pre_img.Pre_Manage(datax, ImgManage::FIND_RED);
                armorfind.Armor_Contours(pre_img.Lightbar, datax);
            }
            double gap = (double)cv::getTickCount() - t;            
            
            stamp_info_on_frame(armorfind.frame0, gap, armorfind.status == ArmorDetect::TRACKING);
            imshow("viewing",armorfind.frame0);
            if(save_sample) outputsample->write(armorfind.frame0);
            //if(armorfind.flag_get)
            //    Send.Angle_Predict(armorfind.target, armorfind.lostNum);
        }
        else{
            //energy code
            //energy.KalmanFind(datax);
            energy.Min2XFind(datax);
            //imshow("frame", energy.binary);
		    imshow("viewing", energy.image);
            if(save_sample) outputsample->write(energy.image);
        }
        Send.set_Mes();//xxxxxx    send mes //......remember optimize
        //Send.Send_Message();
        time(t);
        //keystroke = (char)waitKey(1);
        //if (keystroke == 'e') break;
        waitKey(15);
	}
    //releaseInputHead(camera);
    if(save_sample) outputsample->release();
	return 0;
}