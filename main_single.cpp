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

#define INPUT  "..//video//1.mp4"   //外部输入源
//#define INPUT  "..//video//energy2.mp4"   //外部输入源

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

int main()
{
	InputHead *camera;
	ArmorDetect armorfind;
    armorfind.enable_tracking();
	ImgManage pre_img;
	Energy energy;
	cv::Mat datax;
	armorfind.parameters.useIcFeature = false;
	armorfind.parameters.max_score_threshhold = 0.30;
    
    bool xxx = true;
	long int last_count = 0;//计数最后一次图像数
	string extrainput = INPUT;
#ifdef USE_INDUSTRY_CAMERA
	camera = new CameraInput();
#else
    camera = new VideoInput(extrainput);
#endif

    while (xxx)
    {
        //int working_mode = ImgManage::FIND_RED;
        double t = (double)cv::getTickCount();

        scount++;
        camera->read(datax);
        //datax = imread("..//video//2.png");
        //resize(datax,datax,Size(640,480));
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
        bool xx = true;
        waitKey(1);//  add jundge code in this place
        if(xx)//armor mode
        {
            if (armorfind.status == ArmorDetect::TRACKING) {
                armorfind.tracking(datax);
            } 
            else {
                pre_img.Pre_Manage(datax, ImgManage::FIND_RED);
                armorfind.Armor_Contours(pre_img.Lightbar, datax);
            }
            imshow("viewing",armorfind.frame0);
            //if(armorfind.flag_get)
            //    Send.Angle_Predict(armorfind.target, armorfind.lostNum);
        }
        else{
            //energy code
            energy.start(datax);
            energy.get_target_Kalman();
        }
        Send.set_Mes();//xxxxxx    send mes //......remember optimize
        //Send.Send_Message();
        time(t);
        waitKey(20);
	}
	return 0;
}