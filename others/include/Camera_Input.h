//
// Created by UNIVERSE on 2020/4/19.
//

#include "Image_Input.h"
#include "CameraApi.h"

#ifndef FINDBLUE_CAMERA_INPUT_H
#define FINDBLUE_CAMERA_INPUT_H
#define PI  3.1416

using namespace cv;
using namespace std;


class CameraInput : public InputHead {
private:
    unsigned char*          g_pRgbBuffer;      //处理后图像输出的数据缓存区地址
    int                     iCameraCounts = 1; //摄像头数量
    int                     iStatus = -1;      //相机初始化返回值
    tSdkCameraDevInfo       tCameraEnumList;   //相机列表
    int                     hCamera;           //相机句柄
    tSdkCameraCapbility     tCapability;       //设备描述信息
    tSdkFrameHead           sFrameInfo;        //输入图像的帧头信息
    BYTE*                   pbyBuffer;         //输出图像数据的缓冲区地址
    IplImage*               iplImage = NULL;
    int                     channel = 3;

public:
    CameraInput();
    bool init() final;
    bool read(cv::Mat &src) final;
    ~CameraInput();
};


#endif //FINDBLUE_CAMERA_INPUT_H
