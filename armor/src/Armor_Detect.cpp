#include"Armor_Detect.h"


void ArmorDetect::Armor_Contours(vector<RotatedRect>& LightBar, cv::Mat& frame)
{
	frame0 = frame.clone();
	//cv::imshow("frame0", frame0); cv::waitKey(1);

    // imshow("4.DstImg",DstImg);
	//----------------------------------filtrate false lightbar--------------------------------------//
	//----------------------------------filtrate false lightbar--------------------------------------//

	//-------------------------------------Armor detect----------------------------------------------//
	vector<Armor> armor;
	Detect(LightBar, armor);
	//GetScore(armor);
	//Choice(armor);

	ArmorGrading(armor);
	final_choice(armor);

	imshow("viewing",frame0);
	if (status == INIT_TRACKING) tracking(frame0);
}

float ArmorDetect::AngleConvert(float RowAngle)    //convert to poleAngle
{
	float NewAngle;
	if (RowAngle <= 90)
		NewAngle = 90 - RowAngle;
	else
		NewAngle = 270 - RowAngle;
	return NewAngle;
}

float ArmorDetect::AngleConvert2(float poleAngle)   //convert poleAngle to rotatedRect angle
{
	float RowAngle;
	if (poleAngle <= 90)
		RowAngle = 90 - poleAngle;
	else
		RowAngle = 270 - poleAngle;
	return RowAngle;
}


vector<Armor> ArmorDetect::Detect(vector<RotatedRect>& LightBar, vector<Armor>& result)
{
	if (LightBar.size() < 2)
		return result;
	// cout <<"???" << endl;
	static int imgNum = 0;
	float AngleI, AngleJ;
	Armor armor;
#pragma omp parallel for
	for (uint i = 0; i < LightBar.size() - 1; i++)
	{
		Mat framex = frame0.clone();
		//#pragma omp parallel for
		for (uint j = i + 1; j < LightBar.size(); j++)
		{
			AngleI = AngleConvert(LightBar[i].angle);
			AngleJ = AngleConvert(LightBar[j].angle);
			if (debug)
			{
				//cout<<"abs(AngleI - AngleJ):"<<abs(AngleI - AngleJ)<<endl;
				//cout <<"100 *LightBar的height)"<< 100 * max(LightBar[i].size.height, LightBar[j].size.height) << endl;
				//cout << "HeightThres * min(LightBar[i].size.height, LightBar[j].size.height)" << HeightThres* min(LightBar[i].size.height, LightBar[j].size.height) << endl;

				//cout << "100 *LightBar的size.width" << 100 * max(LightBar[i].size.height, LightBar[j].size.height) << endl;
				//cout << "WidthThres * min(LightBar[i].size.width, LightBar[j].size.width)" << WidthThres * min(LightBar[i].size.width, LightBar[j].size.width) << endl;
				circle(framex, LightBar[i].center, 2, Scalar(255, 0, 0), 2);
				circle(framex, LightBar[j].center, 2, Scalar(255, j*10+100, 0), 2);
				imshow("circle", framex); waitKey(1);
			}

			if (abs(AngleI - AngleJ) < AngleThres &&
				100 * max(LightBar[i].size.height, LightBar[j].size.height) < HeightThres * min(LightBar[i].size.height, LightBar[j].size.height) 
				&& 100 * max(LightBar[i].size.width, LightBar[j].size.width) < WidthThres * min(LightBar[i].size.width, LightBar[j].size.width) 
				&& abs(LightBar[i].center.x-LightBar[j].center.x) < ArmorRatiox * max(LightBar[i].size.height, LightBar[j].size.height)
				&& abs(LightBar[i].center.y - LightBar[j].center.y) < ArmorRatioy
				)
			{
				armor.height = heightCoeff * (LightBar[i].size.height + LightBar[j].size.height) / 2;
				armor.width = sqrt(pow((LightBar[i].center.x - LightBar[j].center.x), 2) + pow((LightBar[i].center.y - LightBar[j].center.y), 2));

				if (armor.width > armor.height* ArmorProMin&& armor.width < ArmorProMax * armor.height)
				{
					armor.center.x = (LightBar[i].center.x + LightBar[j].center.x) / 2;
					armor.center.y = (LightBar[i].center.y + LightBar[j].center.y) / 2;
					armor.dipAngle = 180 / PI * atan(abs(LightBar[i].center.y - LightBar[j].center.y) / abs(LightBar[i].center.x - LightBar[j].center.x));
					armor.angle = 90 - armor.dipAngle;

					armor.PoleAngle = AngleConvert(armor.angle);
					armor.area = armor.width * armor.height;
					//armor.distance=
					armor.score = aWeight * armor.area - bWeight * abs(armor.YawAngle) - cWeight * abs(armor.PitchAngle);
					//cout << "armor.score:" << armor.score << endl;

					if (debug)
					{
						//armor.CalcPoints(armor.pt);
						//DrawArmor(framex, armor.pt);
					}
					if (LightBar[i].center.x < LightBar[j].center.x)
					{
						armor.leftBar = LightBar[i];
						armor.rightBar = LightBar[j];
						armor.leftBar.size.height = armor.height;
						armor.rightBar.size.height = armor.height;
						armor.lbAngle = AngleI;
						armor.rbAngle = AngleJ;
					}
					else
					{
						armor.leftBar = LightBar[j];
						armor.rightBar = LightBar[i];
						armor.leftBar.size.height = armor.height;
						armor.rightBar.size.height = armor.height;
						armor.lbAngle = AngleJ;
						armor.rbAngle = AngleI;
					}

					//----------------------------------cut armor picture-----------------------------------------//
					if (SVM.use_SVM(frame0, armor))
					//else if (SVM.Euclidean_distance(frame0, armor))	//use euclidean distance
					{
					    flag_get = true;
						result.push_back(armor);
					}
				}
			}
		}
	}
	return result;
}

void ArmorDetect::GetScore(vector<Armor>& armor)
{
	
	int HighestScore = -100000, SecondHighScore = -100000;
	for (uint i = 0; i < armor.size(); i++)
	{
		if (armor[i].data == 1)
		{
			armor[i].score += 1.3 * armor[i].center.y;
			HighestScore = armor[i].score;
			break;
		}
		if (armor[i].data == 3 || armor[i].data == 4 || armor[i].data == 5)
		{
			if (armor[i].width / armor[i].height < 0.35)
				armor[i].score = -10150000;
		}
		//printf("armor.size() = %d\n",armor.size());
		if (armor[i].data == lastdata && lastfire == 1)
		{
			choice1 = i;
			HighestScore = armor[i].score;
			break;
		}
		if (armor[i].score > HighestScore&& armor[i].height > ArmorHeightMin)
		{
			choice1 = i;
			HighestScore = armor[i].score;
		}
	}
	for (uint i = 0; i < armor.size(); i++)
	{
		if (i != choice1 && armor[i].score > SecondHighScore&& armor[i].height > ArmorHeightMin)
		{
			choice2 = i;
			SecondHighScore = armor[i].score;
		}
	}
	
}

void ArmorDetect::ArmorGrading(vector<Armor>& armors)
{
	cout << armors.size() <<" armors received\n";
	
	for (int i = 0; i < armors.size(); i++)
	{
		armors[i].score = 0;
		cout << "- Now Armor " << i << ": prescore: "<< armors[i].score;
		float area = armors[i].area;

		//PRESET SCORES
		//armors[i].score += preset_scores[armors[i].data] * 2;

		//SCORE OF AREA
		// use function ln(100*x+1) 's value in the range from 0 to 4
		// the value of this function is from 0 to 6
		area = (area / (frame_cols * frame_rows)) * 5; //0-5
		//armors[i].score += log(100 * area + 1) * 10;
		armors[i].score += area * area * 10; //from 0 to 250

		//SCORE OF POSITION [funtion: 1/x ]
		float pos_bias = abs(armors[i].center.x - frame_cols / 2);
		pos_bias = (pos_bias / (frame_cols / 2)) * 4 + 0.25; //from 0.25 to 4.25 
		//armors[i].score += (1 / pos_bias) * 20;
		armors[i].score += pos_bias * 20; //from 5 to 85

		//with regard to last seen position
		if(previous_exist) {
			armors[i].CalcPoints(armors[i].pt);
			int center_x = (armors[i].pt[2].x + armors[i].pt[0].x) / 2;
			int center_y = (armors[i].pt[2].y + armors[i].pt[0].y) / 2;
			if(abs(last_center.x - center_x) < 0.03 * frame_cols && abs(last_center.y - center_y) < 0.03 * frame_cols){
				armors[i].score += 40;
			}
		}
		//armors[i].score += armors[i].distance;
		printf("now score: %f\n", armors[i].score);
	}
	sort(armors.begin(),armors.end());
}


void ArmorDetect::Choice(vector<Armor>& armor)
{
	Rect aimAreaRect(Point((0.5 - aimAreaWidth) * imgSize.width, (0.5 - aimAreaHeight) * imgSize.height), Point((0.5 + aimAreaWidth) * imgSize.width, (0.5 + aimAreaHeight) * imgSize.height));

	//Send.inAimArea = false;
	cout << "armor.size()=" << armor.size() << endl;
	if (armor.size() == 0)
	{

		//----------don't attack--------//
		//Send.Set_Miss();
		//data[8]='0'+count_send_times%2;
		//Send.count_send_times++;

		cout << "data = 0,0,0,0,0,0---no target" << endl;
		if (antiShake > shakeThres)
		{
		}
	}
	else
	{
		//Send.success++;
		/*
		if (armor.size() == 1)
			choice = choice1;
		else
		{
			if (abs(armor[choice1].center.x - armor[choice2].center.x) < 8 && abs(armor[choice1].center.y - armor[choice2].center.y) > 5)
			{
				if (armor[choice1].center.y > armor[choice2].center.y)
					choice = choice1;
				else
					choice = choice2;
			}
			else if (armor[choice1].score >= 1.1 * armor[choice2].score)
				choice = choice1;
			else
				choice = armor[choice1].center.x >= armor[choice2].center.x ? choice1 : choice2;
		}
		*/
		//choice = tt[armor.size() - 1].index;
		choice = armor.size()-1;
		armor[choice].CalcPoints(pt);
		if (debug)
		{
			//DrawArmor(frame, pt);
			//                    vector<Armor> otherArmor=armor;
			//                    for(int i=0;i<otherArmor.size();i++)
			//                    {
			//                        if(otherArmor[i]==armor[choice])
			//                            break;
			//                    }
			//                    otherArmor.erase(otherArmor.begin()+i);
			//                    for(int i=0;i<otherArmor.size();i++)
			//                    {
			//                        otherArmor[i].CalcPoints(pt);
			//                        for(int i=0;i<4;i++)
			//                        {
			//                            line(frame,pt[i],pt[(i+1)%4],Scalar(0,255,255),1);
			//                        }
			//                    }
		}
		DrawArmor(frame0, pt);
		//cout<<"armor height = "<<armor[choice].height<<endl;
		//cout<<"armor pro = "<<armor[choice].width/armor[choice].height<<endl;

		Armor target = armor[choice];
		//rui
		lastshoot_center_x = target.center.x;
		lastshoot_center_y = target.center.y;
		lastArmor = target;


		//cout << "*************target.height***********:" << target.height << endl;
		//                if(target.height>15 && target.height<20)
		//                    afy=15;
		//                else if(target.height>20 && target.height<25)
		//                    afy=10;
		//                else if(target.height>20 && target.height<25)
		//                    afy=5;
		//                else if(yutarget.height>25 && target.height<33)
		//                    afy=2;
		//                else if(target.height>33 && target.height<40)
		//                    afy=-10;
		//                else if(target.height>40 && target.height<45)
		//                    afy=-25;
		//                else if(target.height>45 && target.height<60)
		//                    afy=-30;
		//                else if(60<target.height)
		//                    afy=-40;
		if (target.height > 40)
		{
			//Send.afy = 2; Send.afx = 2;
		}
		else if (target.height < 25)
		{
			//Send.afy = 6; Send.afx = 0;
		}               //-----------------calibrate yaw----------------------
		//cout << "*************afy***********:" << Send.afy << endl;

		/*if (Send.afx != 0)
		{
			target.center.x = (target.center.x + Send.afx) > 0 ? (target.center.x + Send.afx) : 0;
			target.center.x = (target.center.x + Send.afx) < imgSize.width ? (target.center.x + Send.afx) : imgSize.width;
		}*/
		//-----------------calibrate pitch-------------------
		/*target.center.y = (target.center.y - Send.afy) > 0 ? (target.center.y - Send.afy) : 0;
		target.center.y = (target.center.y - Send.afy) < imgSize.height ? (target.center.y - Send.afy) : imgSize.height;*/


		//                vector<Point2f> pt2;    //something wrong here when copy data
		//                pt2[0]=pt[0];
		//                pt2[1]=pt[1];
		//                pt2[2]=pt[2];
		//                pt2[3]=pt[3];



		int fire = 0;

		//                Rect tRect=boundingRect(pt2);
		//                if((imgSize.width/2>target.center.x-tRect.width/2) && (imgSize.width/2<target.center.x+tRect.width/2) && (imgSize.height/2>target.center.y-tRect.height/2) && (imgSize.height/2<target.center.y+tRect.height/2))
		//                    fire=1;
		//                  else
		//                    fire=0;

						//---------deal with enemy who is shaking body or rotating--------
		if (target.center.x >= aimAreaRect.x && target.center.x <= aimAreaRect.x + aimAreaRect.width
			&& target.center.y <= aimAreaRect.y + 1.5 * aimAreaRect.height && target.center.y >= aimAreaRect.y - aimAreaRect.height)
		{
			SVM.countOut = 0;
			//Send.inAimArea = true;
			antiShake = 0;
		}
		else
			//Send.inAimArea = false;
			if (SVM.countOut < 5)
				fire = 1;


		//cv::circle(frame0, target.center, 2, cv::Scalar(0, 0, 255), 2);


		lastfire = fire;
		lastdata = target.data;
	}
}

Point2d getRectCenter(Rect2f& rect)
{
	Point2d center;
	center.x = rect.x + (rect.width / 2.0);
	center.y = rect.y + (rect.height / 2.0);
	return center;
}
Point2d getRectCenter(Rect2d& rect)
{
	Point2d center;
	center.x = rect.x + (rect.width / 2.0);
	center.y = rect.y + (rect.height / 2.0);
	return center;
}

bool ArmorDetect::generate_bbox(Armor& target, Rect2f& bbox)
{
	//Armor& target = the_target;
		//generate bounding box from pt;
		int delta_x = target.pt[2].x - target.pt[0].x;
		int delta_y = target.pt[2].y - target.pt[0].y;
		int center_x = (target.pt[2].x + target.pt[0].x) / 2;
		int center_y = (target.pt[2].y + target.pt[0].y) / 2;

		/*
		if (center_x < 0.05 * frame_cols || center_x > 0.95 * frame_rows || 
			delta_x * delta_y > 0.10 * frame_cols * frame_rows)// || delta_x * delta_y < 150) 
		{
			return false;
		}*/
		if (delta_x/delta_y > 2.4 
			//|| target.area > 1.2 * delta_y * delta_x
			) {
			return false;
		}
		Point2d box_center(center_x, center_y);
		
		Point2d box_tl(box_center.x - 0.9 * delta_x, box_center.y - 0.8 * delta_y);
		Point2d box_br(box_center.x + 0.9 * delta_x, box_center.y + 0.8 * delta_y);
		bbox = Rect2f(box_tl, box_br);

		if (bbox.tl().x < 2 || bbox.tl().y < 2 || 
			bbox.br().x > frame_cols || bbox.br().y > frame_rows) return false;

	return true;
}

bool ArmorDetect::end_tracking()
{
	if (ecotracker) {
#ifdef USE_MULTI_THREAD
	void *status1;
	int rc = pthread_join(ecotracker->thread_train_, &status1);
	if (rc)
	{
		cout << "Error:unable to join," << rc << std::endl;
		exit(-1);
	}
#endif
		delete ecotracker;
		ecotracker = NULL;
	}
	status = ARMORFIND;
}
void ArmorDetect::tracking(Mat& frame)
{
	static int tot_fail;
	static int tot_tracks;
	static int fails_in_a_raw;

	//if (if_tracking == false) return;
	if (status == INIT_TRACKING)
	{
		if (!generate_bbox(the_target, bbox)){
			end_tracking();
			return;
		}
	}
	/*
	if (bbox.tl().x < 2 || bbox.tl().y < 2 {
		if_tracking = false; 
		if_init = false;
		return;
	}
	if (bbox.br().x > frame.cols || bbox.br().y > 0.95 * frame.rows) {
		if_tracking = false;
		if_init = false;
		return;
	}*/
	
	// The scale of bbox may vary during the process of tracking
	// so these judging statements should be put here.
	if (bbox.width / bbox.height < 0.8 ){
		end_tracking();
		return;
	}

	/*
	if (bbox.area() > 0.38 * frame.rows * frame.cols) {
		if_tracking = false;
		if_init = false;
		return;
	}*/

	bool ok = true;
	if (status == INIT_TRACKING) {
		//tracker = TrackerKCF::create();
		//tracker->init(frame, bbox);
		double timer = (double)getTickCount();

		ecotracker = new eco::ECO();
		ecotracker->init(frame, bbox, parameters);
		cout << "<init time> :" << 1000 * ((double)cv::getTickCount() - timer) / cv::getTickFrequency() << " ms\n";
		cout << "init successfully, bbox:" << bbox << endl;
		//cout << "- frame size: "<<frame.cols<<'*'<<frame.rows;
		//putText(frame, "Init successfully", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

		tot_fail = 0;
		tot_tracks = 1;
		fails_in_a_raw = 0;
		//last_tracked_frames = 16;
		status = TRACKING;
	}
	//should use ELSE IF (or return)
	// since the program needs A frame to init the tracking
	else if (status = TRACKING) {
		//bool ok;
		double timer = (double)getTickCount();
		//ok = tracker->update(frame, bbox);
		ok = ecotracker->update(frame, bbox);
		/*
		if (tot_tracks%10==0) {
			vector<Armor> armor;
			ArmorDetect(Lightbar, armor);
			ArmorGrading(armor);
			final_choice(armor);
			Rect2f tbbox;
			generate_bbox(frame,the_target,tbbox);
			Point2d p1 = getRectCenter(tbbox);
			Point2d p2 = getRectCenter(bbox);
			if(abs(p1.x - p2.x) > 0.03 * frame.cols || abs(p1.y - p2.y) > 0.03 * frame.rows) {
				if_tracking = false;
				if_init = false;
				return;
			}
		}*/
		tot_tracks++;
		//cout<<"tot tracks: "<<tot_tracks<<endl;

		float fps = getTickFrequency() / ((double)getTickCount() - timer);
		if (!ok) {
			tot_fail++;
			++fails_in_a_raw;
			cout << "! Tracking fails ";
			//putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
		}
		else
		{
			cout << "* Tracking       ";
			fails_in_a_raw = 0;
			//cout<<bbox<<endl;
		}
		//putText(frame, "Tracking", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
		//putText(frame, "FPS : " + to_string(int(fps)), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);
		cout<<" @FPS: "<<(int)fps;
		cout << "  <time> :" << 1000 * ((double)cv::getTickCount() - timer) / cv::getTickFrequency() << " ms\n";
	}
/**/
	if (bbox.tl().x < 0 || bbox.tl().y < 0) {
		end_tracking(); 
	}
	if (bbox.br().x > frame.cols || bbox.br().y > frame.rows) {
		end_tracking();
	}
	if (bbox.area() > 0.30 * frame.rows * frame.cols) {
		end_tracking();
	}

	if (ok) rectangle(frame, bbox, Scalar(0, 255, 0), 2, 1); //print in green
	else rectangle(frame, bbox, Scalar(0, 0, 255), 2, 1); //print in red
	circle(frame, getRectCenter(bbox), 3, Scalar(0, 0, 255), -1); //print circle
	
	//double timer = (double)getTickCount();
	imshow("viewing", frame);
	//cout << "  <time> :" << 1000 * ((double)cv::getTickCount() - timer) / cv::getTickFrequency() << " ms\n";
	
	if (waitKey(1) == 27){
		end_tracking();
	}

	if (fails_in_a_raw > 20 || 
		tot_tracks > 300 || 
		tot_fail / tot_tracks > 0.15
		) end_tracking();
	//if (tot_fail > 20) end_tracking();
}
void ArmorDetect::final_choice(vector<Armor>& armor)
{
	if (armor.size() == 0)
	{
		//----------don't attack--------//
		//Send.Set_Miss();
		//data[8]='0'+count_send_times%2;
		//Send.count_send_times++;
        lostNum++;

		cout << "data = 0,0,0,0,0,0---no target" << endl;
		if (antiShake > shakeThres)
		{
		}
	}
	/*else
	{//this place can be optimized
	    choice = tt[armor.size() - 1].index;


        choice = tt[armor.size() - 1].index;
		armor[choice].CalcPoints(pt);
        target = armor[choice];

        if (abs(target.center.x - lastshoot_center_x) < 5 && abs(target.center.y - lastshoot_center_y) < 5)
        {
            lostNum = 0;
        }
        else
            lostNum++;
        lastshoot_center_x = target.center.x;
        lastshoot_center_y = target.center.y;
        lastArmor = target;//optimize

		cout << target.data << " :armor choice:"<<choice;
		DrawArmor(frame0, armor[choice].pt);
	}*/
	else {
		//choice = tt[armor.size() - 1].index;
		//the_target = armor[choice];
		the_target = armor[armor.size()-1];
		//the_target.CalcPoints(pt);

		cout << the_target.data << " :armor choice:"<<choice;
		

		int center_x = (the_target.pt[2].x + the_target.pt[0].x) / 2;
		int center_y = (the_target.pt[2].y + the_target.pt[0].y) / 2;
		if(!previous_exist) {
			last_center = Point2d(center_x,center_y);
			previous_exist = true;
		}
		else {
			//if (abs(last_center.x - center_x) < 0.03 * frame0.cols && abs(last_center.y - center_y) < 0.03 * frame0.cols){
			//	status = INIT_TRACKING;
			//} 
			status = INIT_TRACKING;
			last_center = Point2d(center_x, center_y);			
		}
		DrawArmor(frame0, the_target.pt);
	}
}

void ArmorDetect::DrawArmor(Mat& frame, Point2f pt[])
{
	for (int i = 0; i < 4; i++)
	{
		line(frame, pt[i], pt[(i + 1) % 4], Scalar(0, 0, 255), 1);
	}
	//cv::imshow("xx", frame); cv::waitKey(1);
}
