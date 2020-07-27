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
void releaseInputHead(InputHead* & ptr)
{
    if (ptr) {
        delete (ptr);
        ptr = nullptr;
    }
}

int main()
{
	InputHead *camera;
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
    //VideoWriter output("..//video//sample.mp4",CV_FOURCC('A','V','C','1'),30,Size(640,480));
    //armorfind.disable_tracking();
    //char keystroke = '\0';
    while (xxx)
    {
        //int working_mode = ImgManage::FIND_RED;
        double t = (double)cv::getTickCount();

        scount++;
        bool ok = camera->read(datax);
        //datax = imread("..//video//2.png");
        //resize(datax,datax,Size(640,480));
        if (!ok) {
            releaseInputHead(camera);
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
            imshow("viewing",armorfind.frame0);
            //output.write(armorfind.frame0);
            //if(armorfind.flag_get)
            //    Send.Angle_Predict(armorfind.target, armorfind.lostNum);
        }
        else{
            //energy code
            //energy.KalmanFind(datax);
            energy.Min2XFind(datax);
            //imshow("frame", energy.binary);
		    imshow("viewing", energy.image);
            //output.write(energy.image);
        }
        Send.set_Mes();//xxxxxx    send mes //......remember optimize
        //Send.Send_Message();
        time(t);
        //keystroke = (char)waitKey(1);
        //if (keystroke == 'e') break;
        waitKey(20);
	}
    releaseInputHead(camera);
    //output.release();
	return 0;
}