//
// Created by UNIVERSE on 2020/4/19.
//

#include "../include/Camera_Input.h"
#ifdef USE_INDUSTRY_CAMERA
using namespace cv;
CameraInput::CameraInput()
{
    init();
}

bool CameraInput::init()
{

    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts);

    //没有连接设备
    if (iCameraCounts == 0)
    {
        std::cout << "no device was found!\n";
    }
    else
    {
        iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);

        //初始化失败
        while (iStatus != CAMERA_STATUS_SUCCESS)
        {
            std::cout << "initialize failed!\n";
            iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);
        }

        //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
        CameraGetCapability(hCamera, &tCapability);
        //printf("iFrameSpeedDesc = %d\n",tCapability.iFrameSpeedDesc);         //frame rate option number,when set,iframeSpeed range (0,iFrameSpeedDec-1)
        CameraSetFrameSpeed(hCamera, 2);          //set frame rate,the bigger,the quicker(0~2)
        CameraSetAnalogGain(hCamera, 4);           //mo ni zeng yi
        int analoggain;
        CameraGetAnalogGain(hCamera, &analoggain);
        std::cout << "analoggain = " << analoggain << std::endl;

        CameraSetAeState(hCamera, false);             //设置为手动曝光
        double epTime = 2000;                                   //单位为微秒
        if (CameraSetExposureTime(hCamera, epTime) == 0)
            std::cout << "set exposure successful!\n";
        else
            std::cout << "set exposure failed!\n";
        double expTime = 0;
        CameraGetExposureTime(hCamera, &expTime);
        std::cout << "explore time = " << expTime << "ms" << std::endl;

        g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 3);  //set buffer
        //g_readBuf = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);
        //cout << "111" <<endl;
        /*让SDK进入工作模式，开始接收来自相机发送的图像
        数据。如果当前相机是触发模式，则需要接收到
        g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);  //set buffer
        //g_readBuf = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

        /*让SDK进入工作模式，开始接收来自相机发送的图像
        数据。如果当前相机是触发模式，则需要接收到
        触发帧以后才会更新图像。    */
        CameraPlay(hCamera);

        if (tCapability.sIspCapacity.bMonoSensor) {
            channel = 1;
            CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
        }
        else {
            channel = 3;
            CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_BGR8);
        }
    }
    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    return true;
}
bool CameraInput::read(cv::Mat& input)
{
    double t = cv::getTickCount();
    int a = CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 100);
    if (a == CAMERA_STATUS_SUCCESS)    //第四个参数为超时时间，单位为ms
    {

        CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);
        if (iplImage)
        {
            cvReleaseImageHeader(&iplImage);
        }

        iplImage = cvCreateImageHeader(cvSize(sFrameInfo.iWidth, sFrameInfo.iHeight), IPL_DEPTH_8U, channel);
        cvSetData(iplImage, g_pRgbBuffer, sFrameInfo.iWidth * channel);//此处只是设置指针，无图像块数据拷贝，不需担心转换效率
        cv::Mat Iimag = cv::cvarrToMat(iplImage);
        double resizet = cv::getTickCount();

        resize(Iimag, input, cv::Size(640, 480));
        resizet = ((double)cv::getTickCount() - resizet) / cv::getTickFrequency();
        //在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
        //否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
        CameraReleaseImageBuffer(hCamera, pbyBuffer);
    }
    else {
        CameraReleaseImageBuffer(hCamera, pbyBuffer);
        //Linux System
        /*
        CameraReConnect(hCamera);
        std::cout << "CameraGetImageBuffer123: " << a << std::endl;*/
    }
    t = ((float)cv::getTickCount() - t) / cv::getTickFrequency();
    std::cout << "duqu: " << t << std::endl;
    return true;
}

CameraInput::~CameraInput()
{
    CameraUnInit(hCamera);
    //注意，现反初始化后再free
    free(g_pRgbBuffer);
}
#endif