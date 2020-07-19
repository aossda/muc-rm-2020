#include"../include/Num_Select.h"

Num_Select::Num_Select(void)
{
	Mat  tem_image;
	for (int i = 0; i < 8; i++)
	{
		tem_image = imread(filename + to_string(i + 1) + ".jpg", 0);
		tmp[i] = tem_image;
	}
}

bool Num_Select::depart_img(cv::Mat& frame, Armor& armor)
{
	armor.height *= 1.5;
	armor.CalcPoints(armor.pt);
	Point2f dstPt[4];
	//DrawPoint(frame, armor.pt);

	//for (int i = 0; i < 4; i++)
	//{
	//	armor.pt[i].x *= 2;
	//	armor.pt[i].y *= 2;
	//}//     ??????想不起来为什么有这个以及其作用

	//DrawPoint(frame, armor.pt);

	dstPt[0] = Point2f(0, 0);
	dstPt[1] = Point2f(0, imgHeight);
	dstPt[2] = Point2f(imgWidth, imgHeight);
	dstPt[3] = Point2f(imgWidth, 0);
	Mat transMat = Mat::zeros(3, 3, CV_32FC1);
	transMat = getPerspectiveTransform(armor.pt, dstPt);
	warpPerspective(frame, aImage, transMat, Size(imgWidth, imgHeight));

	//                        if(ifSaveVideo && fcount<30000)
	//                        {
	//                            Mat armorImg;
	//                            resize(aImage,armorImg,Size(2*imgWidth,2*imgHeight));
	//                            video2<<armorImg;
	//                        }

	aImage.convertTo(aImage, -1, 2, 1);

	if (debug)
	{
		imshow("aImage", aImage); cv::waitKey(1);
		Save_Picture(aImage);
	}
	return true;
}

bool Num_Select::use_SVM(cv::Mat& frame, Armor& armor)
{
	depart_img(frame, armor);
	hog->compute(aImage, descriptors, Size(1, 1), Size(0, 0));
	Mat res(1, descriptors.size(), CV_32FC1);
	float* data_res = (float*)res.data;
	for (uint i = 0; i < descriptors.size(); i++)
	{
		data_res[i] = descriptors[i];
	}

	Mat resultMat(1, 1, CV_32FC1);
	svm->predict(res, resultMat);
	float* data = (float*)resultMat.data;

	//rui
	if (data[0] != 0)
	{
		armor.data = data[0];
		cout << "data:" << data[0] << endl;
		//                            if(debug)
		//                            {
		//                                char ID[1];
		//                                ID[0]=(int)data[0]+'0';
		//                                putText(frame,ID,Point(armor.center.x,armor.center.y),FONT_HERSHEY_SIMPLEX,0.3,Scalar(0,255,0));
		//                            }
		return true;
	}
	else
	{
		return false;
	}
	//                        if(lostNum<10)
	//                            result.push_back(lastArmor);
}

void Num_Select::Save_Picture(cv::Mat aImage)
{
	if (imgNum < 2000)
	{
		//                            char imgName[100];
		//                            sprintf(imgName, "./ArmorImg/%d.jpg", imgNum);
		//                            imwrite(imgName, aImage);
		//                            cout<<++imgNum<<endl;
		cv::Mat aimaa;
		resize(aImage, aimaa, cv::Size(40, 40));
		oss << "..//img//";//"/home/nvidia/opencv/findred/image/";
		imgNum += 1;
		oss << imgNum << ".jpg";
		imwrite(oss.str(), aimaa);//保存
		oss.str("");//每次用了oss字符串后，清空字符串内容

		cout << "ing..\n";

	}
}

bool Num_Select::Euclidean_distance(Mat& src, Armor& armor)
{
	depart_img(src, armor);
	Mat gray;
	cvtColor(aImage, gray, CV_BGR2GRAY);
	Rect cutRect(7, 1, 25, 38);
	Mat srcCut = gray(cutRect);
	Mat srcThr;
	adaptiveThreshold(srcCut, srcThr, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 13, -1);//13,-3 or 11,-1
	Mat srcOpen1;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(srcThr, srcOpen1, MORPH_OPEN, element1);
	Scalar meanVal = mean(srcOpen1);
	float mean_val = meanVal.val[0];
	if (mean_val < 37)
	{
		return 0;
	}
	else
	{
		Rect rect = boundingRect(srcOpen1);
		Mat srcOpen0;
		Mat element0 = getStructuringElement(MORPH_RECT, Size(2, 2));
		morphologyEx(srcThr, srcOpen0, MORPH_OPEN, element0);
		Mat srcRect = srcOpen0(rect);
		int fxy = 40.0 / rect.height;
		Mat rectResize;
		resize(srcRect, rectResize, Size(rect.width + 5, 40), INTER_LINEAR);
		Mat numPro;
		int left;
		left = (40 - rect.width - 5) / 2;
		copyMakeBorder(rectResize, numPro, 0, 0, left, 40 - left - rect.width - 5, BORDER_CONSTANT, 0);
		int min_odis = norm(numPro, tmp[0], CV_L2);
		int a, n = 0, num = 1;
		for (int i = 0; i < 8; i++)
		{
			n = i;
			a = norm(numPro, tmp[n], CV_L2);
			if (a < min_odis)
			{
				min_odis = a;
				num = i + 1;
			}
		}
		return num;
		if (num != 0)
		{
			armor.data = num;
			return true;
		}
		else
		{
			return false;
		}
	}
}


void Num_Select::DrawPoint(cv::Mat frame, cv::Point2f* pt)
{
	if (debug)
	{
		circle(frame, pt[0], 1, Scalar(0, 0, 255), 2);
		circle(frame, pt[1], 1, Scalar(0, 0, 255), 2);
		circle(frame, pt[2], 1, Scalar(0, 0, 255), 2);
		circle(frame, pt[3], 1, Scalar(0, 0, 255), 2);
		imshow("point_res", frame); waitKey(1);
	}
}