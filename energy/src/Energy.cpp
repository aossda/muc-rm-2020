//
// Created by UNIVERSE on 2020/5/13.
//

#include "Energy.h"

Energy::Energy()
{
    KalmanInit();
}

void Energy::KalmanInit()
{
	KF.init(stateNum, measureNum, 0);
	processNoise = Mat (stateNum, 1, CV_32F); 
	measurement = Mat::zeros(measureNum, 1, CV_32F);

	KF.transitionMatrix = (Mat_<float>(stateNum, stateNum) << 1, 0, 1, 0,
		0, 1, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1);

	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-5));
	setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(KF.errorCovPost, Scalar::all(1));
	randn(KF.statePost, Scalar::all(0), Scalar::all(0.1));
}
cv::Point2d Energy::KalmanFind(cv::Mat & image)
{
    image.copyTo(binary);

    resize(image, image, Size(image.cols * 0.7, binary.rows * 0.7));
	resize(binary, binary, Size(binary.cols * 0.7, binary.rows * 0.7));
        
    cvtColor(image, image, COLOR_BGR2GRAY);

	threshold(image, image, 80, 255, THRESH_BINARY);        

	dilate(image, image, Mat());
	dilate(image, image, Mat());

	floodFill(image, Point(5, 50), Scalar(255), 0, FLOODFILL_FIXED_RANGE);

	threshold(image, image, 80, 255, THRESH_BINARY_INV);

	vector<vector<Point>> contours;
	findContours(image, contours, RETR_LIST, CHAIN_APPROX_NONE);
	for (size_t i = 0; i < contours.size(); i++) {

	    vector<Point> points;
	    double area = contourArea(contours[i]);
	    if (area < 50 || 1e4 < area) continue;
		drawContours(image, contours, static_cast<int>(i), Scalar(0), 2);

		points = contours[i];
		RotatedRect rrect = fitEllipse(points);
		cv::Point2f* vertices = new cv::Point2f[4];
		rrect.points(vertices);

		float aim = rrect.size.height / rrect.size.width;
		if (aim > 1.7 && aim < 2.6) {
			for (int j = 0; j < 4; j++) {
				cv::line(binary, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0), 4);
			}
			float middle = 100000;

			for (size_t j = 1; j < contours.size(); j++) {

				vector<Point> pointsA;
				double area = contourArea(contours[j]);
				if (area < 50 || 1e4 < area) continue;

				pointsA = contours[j];

				RotatedRect rrectA = fitEllipse(pointsA);

				float aimA = rrectA.size.height / rrectA.size.width;

				if (aimA > 3.0) {
					float distance = sqrt((rrect.center.x - rrectA.center.x) * (rrect.center.x - rrectA.center.x) +
						                  (rrect.center.y - rrectA.center.y) * (rrect.center.y - rrectA.center.y));

					if (middle > distance)
						middle = distance;
				}
			}
			if (middle > 60) {                            
				cv::circle(binary, Point(rrect.center.x, rrect.center.y), 10, cv::Scalar(0, 0, 255), 4);
				Mat prediction = KF.predict();
				Point predict_pt = Point((int)prediction.at<float>(0), (int)prediction.at<float>(1));

				measurement.at<float>(0) = (float)rrect.center.x;
				measurement.at<float>(1) = (float)rrect.center.y;
				KF.correct(measurement);

				circle(binary, predict_pt, 3, Scalar(34, 255, 255), -1);

				rrect.center.x = (int)prediction.at<float>(0);
				rrect.center.y = (int)prediction.at<float>(1);
                return Point2d(rrect.center.x, rrect.center.y);
			}
		}
	}
    if (debug)
    {
        imshow("binary", binary);
        imshow("view", image);
        cout<<"target :"<< rrect.center <<endl;
        waitKey(1);
    }
    return Point2d(0,0);
}

cv::Point2d Energy::Min2XFind(cv::Mat & image)
{
	//image.copyTo(binary);
    binary = image.clone();
	//resize(image, image, Size(image.cols * 0.7, image.rows * 0.7));
	//resize(binary, binary, Size(binary.cols * 0.7, binary.rows * 0.7));

	vector<Mat> channels;
	split(binary, channels);
	threshold(channels.at(2) - channels.at(0), binary, 100, 255, CV_THRESH_BINARY_INV);

    Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat element2 = getStructuringElement(MORPH_RECT, Size(25, 25));
    morphologyEx(binary, binary, MORPH_OPEN, element1);
    floodFill(binary, Point(0, 0), Scalar(0));
    morphologyEx(binary, binary, MORPH_CLOSE, element2);


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(binary, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    int area[25] = { 0 };
    for (int i = 0; i < hierarchy.size(); i++)
    {
        area[i] = contourArea(contours[i]);
        if (area[i] < 2000)
        {
            Point2f rect[4];
            RotatedRect box1 = minAreaRect(Mat(contours[i]));
            circle(image, Point(box1.center.x, box1.center.y), 5, Scalar(255, 0, 0), -1, 8);
            box1.points(rect);
            for (int j = 0; j < 4; j++)
            {
                line(image, rect[j], rect[(j + 1) % 4], Scalar(0, 255, 0), 2, 8);
            }

            points.push_back(box1.center);
            Point2d c;
            double r = 0;
            LeastSquaresCircleFitting(points, c, r);
            circle(image, c, r, Scalar(0, 0, 255), 2, 8); //Draw the circle
            circle(image, c, 5, Scalar(255, 0, 0), -1, 8);
        }
    }
    if(debug)
    {
        imshow("binary", binary);
        imshow("view", image);
        waitKey(1);
    }
    return target;
}

//拟合圆函数
int Energy::LeastSquaresCircleFitting(vector<cv::Point2d>& m_Points, cv::Point2d& Centroid, double& dRadius)
{
    if (!m_Points.empty())
    {
        int iNum = (int)m_Points.size();
        if (iNum < 3)	return 1;
        double X1 = 0.0;
        double Y1 = 0.0;
        double X2 = 0.0;
        double Y2 = 0.0;
        double X3 = 0.0;
        double Y3 = 0.0;
        double X1Y1 = 0.0;
        double X1Y2 = 0.0;
        double X2Y1 = 0.0;
        vector<cv::Point2d>::iterator iter;
        vector<cv::Point2d>::iterator end = m_Points.end();
        for (iter = m_Points.begin(); iter != end; ++iter)
        {
            X1 = X1 + iter->x;
            Y1 = Y1 + iter->y;
            X2 = X2 + iter->x * iter->x;
            Y2 = Y2 + iter->y * iter->y;
            X3 = X3 + iter->x * iter->x * iter->x;
            Y3 = Y3 + iter->y * iter->y * iter->y;
            X1Y1 = X1Y1 + iter->x * iter->y;
            X1Y2 = X1Y2 + iter->x * iter->y * iter->y;
            X2Y1 = X2Y1 + iter->x * iter->x * iter->y;
        }
        double C = 0.0;
        double D = 0.0;
        double E = 0.0;
        double G = 0.0;
        double H = 0.0;
        double a = 0.0;
        double b = 0.0;
        double c = 0.0;
        C = iNum * X2 - X1 * X1;
        D = iNum * X1Y1 - X1 * Y1;
        E = iNum * X3 + iNum * X1Y2 - (X2 + Y2) * X1;
        G = iNum * Y2 - Y1 * Y1;
        H = iNum * X2Y1 + iNum * Y3 - (X2 + Y2) * Y1;
        a = (H * D - E * G) / (C * G - D * D);
        b = (H * C - E * D) / (D * D - G * C);
        c = -(a * X1 + b * Y1 + X2 + Y2) / iNum;
        double A = 0.0;
        double B = 0.0;
        double R = 0.0;
        A = a / (-2);
        B = b / (-2);
        R = double(sqrt(a * a + b * b - 4 * c) / 2);
        Centroid.x = A;
        Centroid.y = B;
        dRadius = R;
        return 0;
    }
    else
        return 1;
    return 0;
}