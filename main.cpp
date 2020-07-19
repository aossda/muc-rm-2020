#include<opencv2/opencv.hpp>
#include<omp.h>
#include<opencv2/core.hpp>
#include<iostream>
#include <unistd.h>
#include<pthread.h>
#include"others/include/Mes_Manage.h"
#include"armor/include/Armor_Detect.h"
#include"armor/include/Img_Manage.h"

#include"eco.hpp"
#include <opencv2/tracking.hpp>
#include <fftw3.h>
#ifdef USE_INDUSTRY_CAMERA
#include"Camera_Input.h"
#else
#include"others/include/Video_Input.h"
#endif

#define INPUT  "video//1.mp4"   //外部输入源

using namespace std;

pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
cv::Mat Iimag, pre_red, pre_binary;
bool cap_flag = 0;

void time(double &t)
{
    cout << "<time> :" << 1000 * ((double)cv::getTickCount() - t) / cv::getTickFrequency()<<" ms\n\n\n\n";
}

void* readcamera(void*)
{
    int scount = 0;
    cv::Mat image;
    InputHead *camera;
    string extrainput = INPUT;
#ifdef USE_INDUSTRY_CAMERA
    camera = new CameraInput();
#else
    camera = new VideoInput(extrainput);
#endif

    while (true)
    {
        scount+=1;

        camera->read(image);
        if (scount <= 250)//		跳过前10帧图像
            continue;
        cap_flag=true;
        pthread_mutex_lock(&my_mutex);

        Iimag = image.clone();
        if (cap_flag == true)
            pthread_cond_signal(&cond);
        pthread_mutex_unlock(&my_mutex);

        usleep(10);
    }
    return 0;
}

void* ProcessImg(void*)
{
    ArmorDetect armorfind;
    ImgManage pre_img;
    cv::Mat datax;
    while (1)
    {
        int working_mode = ImgManage::FIND_RED;
        double t = (double)cv::getTickCount();
        pthread_mutex_lock(&my_mutex);
        if (cap_flag == false)
        {
            pthread_cond_wait(&cond, &my_mutex);
        }
        datax = Iimag.clone();
        cap_flag = false;

        pthread_mutex_unlock(&my_mutex);
        //lock and get image

        bool xx = true;
        waitKey(1);// add jundge code in this place
        if(xx)//armor mode
        {
            if (armorfind.status == ArmorDetect::ARMORFIND) {
                pre_img.Pre_Manage(datax, working_mode);
                armorfind.Armor_Contours(pre_img.Lightbar, datax);
            }
            else if (armorfind.status == ArmorDetect::TRACKING)
            {
                armorfind.tracking(datax);
            }
            //if(armorfind.flag_get)
            //    Send.Angle_Predict(armorfind.target, armorfind.lostNum);
        }
        else{
            //energy code
            //energy.start(datax);
            //energy.get_target_Kalman();
        }
        //Send.set_Mes();//xxxxxx    send mes //......remember optimize
        //Send.Send_Message();
        time(t);
    }

}
int main()
{
    //XInitThreads();

    pthread_t tid1, tid2;
    int rc1 = 0, rc2 = 0;

    rc1 = pthread_create(&tid1, NULL, readcamera, NULL);
    if (rc1 != 0)
    {
        cout << "create process thread failed" << endl;
        return 0;
    }
    rc2 = pthread_create(&tid2, NULL, ProcessImg, NULL);
    if (rc2 != 0)
    {
        cout << "create process thread failed" << endl;
        return 0;
    }
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    cout << "exit main!" << endl;
    return 0;
}
