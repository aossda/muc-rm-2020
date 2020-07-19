#include"../include/Mes_Manage.h"


Mes_Manage::Mes_Manage()
{
    //UartInit(Dev, &fd, 9600);
    cout << "fd = " << fd << endl;
    if (fd == -1)
    {
        printf("open serial port failed!\n");
    }
}

void Mes_Manage::Set_Miss()
{
	data[0] = 'a';
	data[1] = '0';
	data[2] = '0';
	data[3] = '0';
	data[4] = '0';
	data[5] = '0';
	data[6] = '0';
	data[7] = '0';
}

void Mes_Manage::Send_Message()
{
    if (!inAimArea && countOut < 200)
    {
        countOut++;
    }
    // cout << "aa" << aa << endl;

     //------------------write serial-------------------//
    /*if (write(fd, data, sizeof(data)) == -1)
        cout << "send failed!\n" << endl;

       else
    */
        printf("send over!\n ");

    printf(data);
    //cv::imshow("frame",frame);

    //if (debug)
    //{

    //    double t = 1000 * ((float)getTickCount() - t) / getTickFrequency();
    //    //cout<<"t = "<<t<<endl;
    //    int f = 1000 / t;
    //    char fps[4];
    //    fps[0] = f / 100 + '0';
    //    fps[1] = f / 10 % 10 + '0';
    //    fps[2] = f % 10 + '0';
    //    //  cout<<fps<<endl;
    //    //putText(frame, fps, Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 0));

    //}

}

void Mes_Manage::saveVideo(cv::Mat frame)
{
    if (ifSaveVideo && fcount < 9000)
    {
        videoFrame = frame.clone();
        video << videoFrame;
        fcount++;
    }
}

void Mes_Manage::Angle_Predict(Armor& target, int &lostNum)//predict
{
    AngleSolver(target, target.YawAngle, target.PitchAngle);
    if(lostNum == 0) {//use old angle
    }
    else if(lostNum <= 5)
    {
        Yaw = target.YawAngle + (target.YawAngle - Yaw) * 0.08;
        Pitch = target.PitchAngle + (target.PitchAngle - Pitch) * 0.08;
    }
    else
    {
        Yaw = target.YawAngle;
        Pitch = target.PitchAngle;
    }
}

void Mes_Manage::set_Mes()
{
    int check = (abs((int)Yaw) + abs((int)Pitch)) % 10;
    data[0] = 'a';
    if (Yaw < 0)
        data[1] = '0';
    else
        data[1] = '1';

    data[2] = (abs(Yaw)) / 10 + '0';
    data[3] = (abs((int)Yaw)) % 10 + '0';
    if (Pitch < 0)
        data[4] = '0';
    else
        data[4] = '1';
    data[5] = (abs(Pitch)) / 10 + '0';
    data[6] = (abs((int)Pitch)) % 10 + '0';
    data[7] = check + '0';
    //std::cout<<"data = "<<data<<std::endl;
}

void Mes_Manage::AngleSolver(Armor armor, float& YawAngle, float& PitchAngle)
{
    YawAngle = atan(Ytan * (2 * armor.center.x - FrameCols) / FrameCols) / PI * 180;
    PitchAngle = atan(Ptan * (FrameRows - 2 * armor.center.y) / FrameRows) / PI * 180;
}


