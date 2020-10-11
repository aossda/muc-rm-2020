#ifndef ARMORCLASS_H
#define ARMORCLASS_H

#include <opencv2/opencv.hpp>
#include <math.h>
using namespace std;
using namespace cv;

#define PI      3.1416

class Armor
{
public:
    Point2f center;
    Point2f pt[4];
    float angle;
    float PoleAngle;
    float dipAngle;
    float width;
    float height;
    float score;
    float distance;
    float YawAngle;
    float PitchAngle;
    float area;
    RotatedRect leftBar;
    RotatedRect rightBar;
    float lbAngle;
    float rbAngle;
    int data;
    void CalcPoints(Point2f pt2[4])//mingw环境报错？？？？
    {
        pt2[0]=Point2f(leftBar.center.x+0.5*leftBar.size.height*cos(PI*lbAngle/180)     //left up
            ,leftBar.center.y-0.5*leftBar.size.height*sin(PI*lbAngle/180));
        pt2[1]=Point2f(leftBar.center.x-0.5*leftBar.size.height*cos(PI*lbAngle/180)     //left down
            ,leftBar.center.y+0.5*leftBar.size.height*sin(PI*lbAngle/180));
        pt2[2]=Point2f(rightBar.center.x-0.5*rightBar.size.height*cos(PI*rbAngle/180)   //right down
            ,rightBar.center.y+0.5*rightBar.size.height*sin(PI*rbAngle/180));
        pt2[3]=Point2f(rightBar.center.x+0.5*rightBar.size.height*cos(PI*rbAngle/180)   //right up
            ,rightBar.center.y-0.5*rightBar.size.height*sin(PI*rbAngle/180));
    }
    bool operator<(Armor& b){
        if (score < b.score) return true;
        else if (area < b.area) return true;
        else return false;
    }
private:
};

#endif // ARMORCLASS_H
