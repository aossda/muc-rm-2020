#include"Img_Manage.h"

using namespace std;
using namespace cv;

void ImgManage::Pre_Manage(cv::Mat &inp, int mode)
{
	
	//if (debug)
	//{
	//	namedWindow("2.binaryImg");
	//	createTrackbar("thres", "2.binaryImg", &BinaryThres, 255,this->Threshold);
	//	namedWindow("3.ExtractBlue");
	//	createTrackbar("thres", "3.ExtractBlue", &redThres, 255, this->ExtractRed);
	//}

	//thresDilate = getStructuringElement(MORPH_RECT, Size(3, 4));
	//thresErode = getStructuringElement(MORPH_RECT, Size(2, 1));
	//colorDilate = getStructuringElement(MORPH_RECT, Size(4, 7));
	//colorErode = getStructuringElement(MORPH_RECT, Size(2, 1));
	//Mat dstDilate = getStructuringElement(MORPH_RECT, Size(3, 4));
	//Mat dstErode = getStructuringElement(MORPH_RECT, Size(2, 1));


	/* Mat img;

	 c >> img;
	 img.copyTo(frame);
	 resize(frame,frame0,Size(1280,960));
	 cout << "  img "<< img.size() << endl;
	 //resize(frame0,frame,imgSize);*/


	 //-------------------------------save image------------------------------------//

//        if(ifSaveVideo && fcount<30000)    //about 10 minutes
//        {
//            //float tt=(float)getTickCount();
//            while(saveFrame);
//            pthread_mutex_lock(&mutex2);
//            saveFrame=true;
//            image2.copyTo(videoFrame);
//            pthread_mutex_unlock(&mutex2);
//            //tt=((float)getTickCount()-tt)/getTickFrequency();
//            //cout<<"save frame time = "<<1000*tt<<"ms"<<endl;
//        }

	//-------------------------------manage------------------------------------//
	Lightbar.clear();
	cv::Mat BinaryImg, colorImg;
	input_operation(inp, BinaryImg, colorImg, mode);
	
	get_dstimg(inp, BinaryImg, colorImg);
	//------------------------------get light bar contours--------------------//
	get_Lightbar();

}

void ImgManage::ContrastAndBright(Mat& img, Mat& g_dstImage)    // change bright and contrast
{
	int rows = img.rows;
	int cols = img.cols;
	int channels = img.channels();
	uchar* pRow = 0;
	uchar* ps = 0;

	g_dstImage = Mat::zeros(img.size(), img.type());
	// ����forѭ����ִ������ g_dstImage(i,j) = a*g_srcImage(i,j) + b
	for (int i = 0; i < rows; ++i)
	{
		// ��ȡ��ָ��
		pRow = img.ptr<uchar>(i);
		ps = g_dstImage.ptr<uchar>(i);
		for (int j = 0; j < cols; ++j)
		{
			if (channels == 1) //��ͨ��
				ps[j] = 0.01 * g_nContrastValue * pRow[j] + g_nBrightValue;
			else if (channels == 3) //��ͨ��
			{
				pRow[j * 3] = 0.01 * g_nContrastValue * pRow[j * 3] + g_nBrightValue;
				pRow[j * 3 + 1] = 0.01 * g_nContrastValue * pRow[j * 3 + 1] + g_nBrightValue;
				pRow[j * 3 + 2] = 0.01 * g_nContrastValue * pRow[j * 3 + 2] + g_nBrightValue;
			}
		}
	}
}

void ImgManage::input_operation(cv::Mat& input, cv::Mat& BinaryImg, cv::Mat& colorImg, int &mode)
{
	//--------------------------------process-------------------------------------//

	frame = input.clone();
	//normalize(frame,frame);
	if (frame.data)
	{
		// imshow("1.RawImg",frame);

		split(frame, RgbChannels);

			//---------------------------------extract high light object------------------------------------//

        cvtColor(frame, gray, CV_RGB2GRAY);
        normalize(gray, res_norm, 0, 200, NORM_MINMAX);
        //frame.copyTo(res_norm);
		//cvtColor(frame, xxx, CV_RGB2GRAY);ContrastAndBright(xxx,gray);
        if(mode==FIND_BLUE)
		    threshold(res_norm, BinaryImg, BinaryThresBlue, 255, CV_THRESH_BINARY);
        else if(mode==FIND_RED)
            threshold(res_norm, BinaryImg, BinaryThresRed, 255, CV_THRESH_BINARY);
		dilate(BinaryImg, BinaryImg, thresDilate);
		erode(BinaryImg, BinaryImg, thresErode);
		//-------------------------------------extract blue------------------------------------//
		//if(mode == FIND_BLUE) subtract(RgbChannels[0], RgbChannels[2], colorImg);       //Bchannel - Rchannel(BGR)
		//else if (mode == FIND_RED) subtract(RgbChannels[2], RgbChannels[0], colorImg); 
		if(mode == FIND_BLUE) subtract(RgbChannels[0], RgbChannels[1], colorImg);       //Bchannel - Rchannel(BGR)
		else if (mode == FIND_RED) subtract(RgbChannels[2], RgbChannels[1], colorImg);   
		if(mode==FIND_BLUE)
		    threshold(colorImg, colorImg, blueThres, 255, CV_THRESH_BINARY);
        else if(mode==FIND_RED)
            threshold(colorImg, colorImg, redThres, 255, CV_THRESH_BINARY);

        if(mode==FIND_RED)
        {
            dilate(colorImg, colorImg, colorDilate);
            erode(colorImg, colorImg, colorErode);
        }
		//}           
		if (debug)
		{
			cv::imshow("Gray", gray);
			cv::imshow("1.RawImg", frame);
            cv::imshow("2.normalize", res_norm);
			cv::imshow("3.binaryImg", BinaryImg);
			cv::imshow("4.ExtractBlue", colorImg);
			cv::waitKey(1);
		}
	}
}

cv::Mat ImgManage::get_dstimg(cv::Mat& input, cv::Mat& BinaryImg, cv::Mat& colorImg)
{
	Mat f_DstImg = Mat::zeros(imgSize, CV_8UC1);

	findContours(colorImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	findContours(BinaryImg, fgcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	lightExam(fgcontours, contours, &f_DstImg, BinaryImg);

	
	            //for(uint i=0;i<contours.size();i++)
	            //{
	            //    Rect LightBarRect=boundingRect(contours[i]);
	            //    if(LightBarRect.height>=LightBarHeightMin)
	            //    {
	            //        //rectangle(frame,rectContours,Scalar(0,0,255),1,8);
	            //        LightBarExtract(LightBarRect,BlueImg,BinaryImg,DstImg);
	            //    }
	            //}

	//dilate(f_DstImg, f_DstImg, dstDilate);
	//erode(f_DstImg, f_DstImg, dstErode);
	
	DstImg = f_DstImg.clone();
	//if(debug)
	    cv::imshow("4.DstImg", f_DstImg); cv::waitKey(1);

	return DstImg;
}

void ImgManage::get_Lightbar()
{
	RotatedRect box;
	RotatedRect bar;

	findContours(DstImg, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (uint i = 0; i < contours2.size(); i++)
	{
		if (contours2[i].size() >= ContoursSizeMin)
		{
			//cout<<"contours2[i].size() = "<<contours2[i].size()<<endl;
			//--------------------fitEllipse---------------------------
			box = fitEllipse(contours2[i]);

			//--------------------minAreaRect---------------------------
			bar = minAreaRect(contours2[i]);
			//bar.angle is negative number between(-90,0),next 3 lines are to describe bar in the same way as box
			bar.angle = (bar.size.width > bar.size.height) ? (90 + bar.angle) : (180 + bar.angle);
			bar.size.width = min(bar.size.width, bar.size.height);
			bar.size.height = max(bar.size.width, bar.size.height);

			//box=bar;//use the result of minAreaRect only
			if (box.size.height < LightBarProMax * box.size.width)
			{
				if (debug)
				{
					ellipse(frame, box, Scalar(0, 0, 255));
				}
				Lightbar.push_back(box);
			}
		}
	}
}

void ImgManage::lightExam(vector<vector<Point>> binarycontours, vector<vector<Point> > colorcontours, Mat* dst, Mat fgImg) {
    #pragma omp parallel for
	for (int i = 0; i < colorcontours.size(); i++) {
		Rect rect = boundingRect(colorcontours[i]);
		for (int j = 0; j < binarycontours.size(); j++) {
			Rect fgrect = boundingRect(binarycontours[j]); //find min bounding rect
			if (isInside(fgrect, rect)) {
#pragma omp parallel for
                for (int y = fgrect.y; y < fgrect.y + fgrect.height; y++) {
					for (int x = fgrect.x; x < fgrect.x + fgrect.width; x++) {
						dst->at<uchar>(y, x) = fgImg.at<uchar>(y, x);
					}
				}
			}
		}
	}
}

bool ImgManage::isInside(Rect &rect1, Rect &rect2)//is r1 in r2?
{
    //if(debug)
    //   printf("is r1 in r2 %d?\n",(rect1 == (rect1&rect2)));
    return (rect1 == (rect1&rect2));
}

void ImgManage::LightBarExtract(Rect LightBarRect, Mat BlueImg, Mat Binaryimg, Mat& DstImg)
{
	int m, n;
	uchar* dstData = (uchar*)DstImg.data;
	uchar* BlueData = (uchar*)BlueImg.data;
	uchar* BinaryData = (uchar*)Binaryimg.data;

	for (int y = LightBarRect.y; y < LightBarRect.y + LightBarRect.height; y++)
	{
		int rowIndex = y * Binaryimg.cols;
		//#pragma omp parallel for
		for (int x = LightBarRect.x; x < LightBarRect.x + LightBarRect.width; x++)
		{
			if (BinaryData[rowIndex + x] == 255)
			{
				bool flag = false;
				for (m = y - KernelY; m <= y + KernelY; m++)
				{
					for (n = x - KernelX; n <= x + KernelX; n++)
					{
						if (m >= 0 && m < Binaryimg.rows && n >= 0 && n < Binaryimg.cols && BlueData[m * BlueImg.cols + n] == 255)
						{
							flag = true;
							break;
						}
					}
					if (flag)
						break;
				}
				if (flag)
					dstData[rowIndex + x] = 255;
			}
		}
	}
}

//void Threshold(int, void*)//debug ��ֵ�� ������ �ص�����
//{
//	Mat gray;
//	cvtColor(frame, gray, CV_RGB2GRAY);
//	threshold(gray, BinaryImg, BinaryThres, 255, CV_THRESH_BINARY);
//	//threshold(RgbChannels[0],BinaryImg,BinaryThres,255,CV_THRESH_BINARY);
//	//imshow("RawBinary",binaryImg);
//	dilate(BinaryImg, BinaryImg, thresDilate);
//	erode(BinaryImg, BinaryImg, thresErode);
//}
//
//void ExtractRed(int, void*)//debug �� ��ֵ�� ������ �ص�����
//{
//	threshold(colorImg, colorImg, redThres, 255, CV_THRESH_BINARY);
//	dilate(colorImg, colorImg, colorDilate);
//	erode(colorImg, colorImg, colorErode);
//}