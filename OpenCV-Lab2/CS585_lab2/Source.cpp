#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>


using namespace cv;
using namespace std;

/**
Function that returns the maximum of 3 integers
@param a first integer
@param b second integer
@param c third integer
*/
int myMax(int a, int b, int c);

/**
Function that returns the minimum of 3 integers
@param a first integer
@param b second integer
@param c third integer
*/
int myMin(int a, int b, int c);

/**
Function that detects whether a pixel belongs to the skin based on RGB values
@param src The source color image
@param dst The destination grayscale image where skin pixels are colored white and the rest are colored black
*/
void mySkinDetect(Mat& src, Mat& dst);

/**
Function that does frame differencing between the current frame and the previous frame
@param src The current color image
@param prev The previous color image
@param dst The destination grayscale image where pixels are colored white if the corresponding pixel intensities in the current
and previous image are not the same
*/
void myFrameDifferencing(Mat& prev, Mat& curr, Mat& dst);

/**
Function that accumulates the frame differences for a certain number of pairs of frames
@param mh Vector of frame difference images
@param dst The destination grayscale image to store the accumulation of the frame difference images
*/
void myMotionEnergy(vector<Mat> mh, Mat& dst);


/**
Function that does some sort of detection of the three image processing
**/
void condefects(vector<Vec4i> convexityDefectsSet, vector<Point> mycontour, Mat &frame);


/** Global Varialbes **/
int thresh = 128;
int max_thresh = 255;


/** Main Function **/
int main()
{

	//----------------
	//a) Reading a stream of images from a webcamera, and displaying the video
	//----------------
	// open the video camera no. 0
	VideoCapture cap(0);

	// if not successful, exit program
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	//create a window called "MyVideoFrame0"
	namedWindow("MyVideo0", WINDOW_AUTOSIZE);
	Mat frame0; //what is the Mat datatype

	// read a new frame from video
	bool bSuccess0 = cap.read(frame0);

	//if not successful, break loop
	if (!bSuccess0)
	{
		cout << "Cannot read a frame from video stream" << endl;//how are you breaking the loop
	}

	//show the frame in "MyVideo" window
	imshow("MyVideo0", frame0);

	//create a window called "MyVideo", "MyVideoMH", and "Skin"
	namedWindow("MyVideo", WINDOW_AUTOSIZE);
	namedWindow("MyVideoMH", WINDOW_AUTOSIZE);
	namedWindow("Skin", WINDOW_AUTOSIZE);
	namedWindow("RockScissorPaper", WINDOW_AUTOSIZE);


	vector<Mat> myMotionHistory;
	Mat fMH1, fMH2, fMH3;
	fMH1 = Mat::zeros(frame0.rows, frame0.cols, CV_8UC1);
	fMH2 = fMH1.clone();
	fMH3 = fMH1.clone();
	myMotionHistory.push_back(fMH1);
	myMotionHistory.push_back(fMH2);
	myMotionHistory.push_back(fMH3);

	while (1)
	{
		// read a new frame from video
		Mat frame;
		bool bSuccess = cap.read(frame);

		//if not successful, break loop
		if (!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		// destination frame
		Mat SkinframeDest;
		SkinframeDest = Mat::zeros(frame.rows, frame.cols, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
		//----------------
		//	b) Skin color detection
		//----------------
		mySkinDetect(frame, SkinframeDest);
		imshow("Skin", SkinframeDest);

		// Convert into binary image using thresholding
		// Documentation for threshold: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html?highlight=threshold#threshold
		// Example of thresholding: http://docs.opencv.org/doc/tutorials/imgproc/threshold/threshold.html
		Mat thres_output;
		threshold(SkinframeDest, thres_output, thresh, max_thresh, 0);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		// Find contours
		// Documentation for finding contours: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=findcontours#findcontours
		findContours(thres_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		cout << "The number of contours detected is: " << contours.size() << endl;

		//Mat frameDest = Mat::zeros(thres_output.size(), CV_8UC3);
		// Find largest contour
		int maxsize = 0;
		int maxind = 0;
		Rect boundrec;
		for (int i = 0; i < contours.size(); i++)
		{
			// Documentation on contourArea: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html#
			double area = contourArea(contours[i]);
			if (area > maxsize) {
				maxsize = area;
				maxind = i;
				boundrec = boundingRect(contours[i]);
			}
		}

		/// Find the convex hull and defect object for each contour
		vector<vector<Point> >hull(contours.size());
		vector<vector<Vec4i>>defects(contours.size());
		vector<vector<int> >inthull(contours.size());
		for (int i = 0; i < contours.size(); i++)
		{
			convexHull(Mat(contours[i]), hull[i], false);
			convexHull(Mat(contours[i]), inthull[i], false);
			if (inthull[i].size() > 3) {
				convexityDefects(contours[i], inthull[i], defects[i]);
			}
		}

		// Draw contours
		// Documentation for drawing contours: http://docs.opencv.org/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=drawcontours#drawcontours
		// Documentation for drawing rectangle: http://docs.opencv.org/modules/core/doc/drawing_functions.html
		rectangle(frame, boundrec, Scalar(0, 255, 0), 1, 8, 0);
		drawContours(frame, hull, maxind, Scalar(0, 0, 255), 2, 8, hierarchy);
		rectangle(SkinframeDest, boundrec, Scalar(255, 255, 255), 1, 8, 0);
		drawContours(SkinframeDest, hull, maxind, Scalar(255, 0, 255), 2, 8, hierarchy);
		//call the condefect function which plot the 
		condefects(defects[maxind], contours[maxind], frame);

		cout << "The area of the largest contour detected is: " << contourArea(contours[maxind]) << endl;
		cout << "-----------------------------" << endl << endl;

		/// Show in a window
		imshow("RockScissorPaper", frame);
		imshow("source", SkinframeDest);
		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}

		// //----------------
		// //	c) Background differencing
		// //----------------


		// //call myFrameDifferencing function
		// myFrameDifferencing(frame0, frame, frameDest);
		// imshow("MyVideo", frameDest);
		// myMotionHistory.erase(myMotionHistory.begin());
		// myMotionHistory.push_back(frameDest);
		// Mat myMH = Mat::zeros(frame0.rows, frame0.cols, CV_8UC1);

		// //----------------
		// //  d) Visualizing motion history
		// //----------------

		// //call myMotionEnergy function
		// myMotionEnergy(myMotionHistory, myMH);


		// imshow("MyVideoMH", myMH); //show the frame in "MyVideo" window
		// frame0 = frame;
		// //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		// if (waitKey(30) == 27)
		// {
		// 	cout << "esc key is pressed by user" << endl;
		// 	break;
		// }

	}
	waitKey(0);
	cap.release();
	return 0;
}

/** Function Definitions **/

//Function that returns the maximum of 3 integers
int myMax(int a, int b, int c) {
	int m = a;
	(void)((m < b) && (m = b));
	(void)((m < c) && (m = c));
	return m;
}

//Function that returns the minimum of 3 integers
int myMin(int a, int b, int c) {
	int m = a;
	(void)((m > b) && (m = b));
	(void)((m > c) && (m = c));
	return m;
}

//Function that detects whether a pixel belongs to the skin based on RGB values
void mySkinDetect(Mat& src, Mat& dst) {
	//Surveys of skin color modeling and detection techniques:
	//Vezhnevets, Vladimir, Vassili Sazonov, and Alla Andreeva. "A survey on pixel-based skin color detection techniques." Proc. Graphicon. Vol. 3. 2003.
	//Kakumanu, Praveen, Sokratis Makrogiannis, and Nikolaos Bourbakis. "A survey of skin-color modeling and detection methods." Pattern recognition 40.3 (2007): 1106-1122.
	for (int i = 0; i < src.rows; i++){
		for (int j = 0; j < src.cols; j++){
			//For each pixel, compute the average intensity of the 3 color channels
			Vec3b intensity = src.at<Vec3b>(i, j); //Vec3b is a vector of 3 uchar (unsigned character)
			int B = intensity[0]; int G = intensity[1]; int R = intensity[2];
			if ((R > 95 && G > 40 && B > 20) && (myMax(R, G, B) - myMin(R, G, B) > 15) && (abs(R - G) > 15) && (R > G) && (R > B)){
				dst.at<uchar>(i, j) = 255;
			}
		}
	}
}

//Function that does frame differencing between the current frame and the previous frame
void myFrameDifferencing(Mat& prev, Mat& curr, Mat& dst) {
	//For more information on operation with arrays: http://docs.opencv.org/modules/core/doc/operations_on_arrays.html
	//For more information on how to use background subtraction methods: http://docs.opencv.org/trunk/doc/tutorials/video/background_subtraction/background_subtraction.html
	absdiff(prev, curr, dst);
	Mat gs = dst.clone();
	cvtColor(dst, gs, CV_BGR2GRAY);
	dst = gs > 50;
	Vec3b intensity = dst.at<Vec3b>(100, 100);
}

//Function that accumulates the frame differences for a certain number of pairs of frames
void myMotionEnergy(vector<Mat> mh, Mat& dst) {
	Mat mh0 = mh[0];
	Mat mh1 = mh[1];
	Mat mh2 = mh[2];

	for (int i = 0; i < dst.rows; i++){
		for (int j = 0; j < dst.cols; j++){
			if (mh0.at<uchar>(i, j) == 255 || mh1.at<uchar>(i, j) == 255 || mh2.at<uchar>(i, j) == 255){
				dst.at<uchar>(i, j) = 255;
			}
		}
	}
}

//Function that determines rock, paper, scissors
void condefects(vector<Vec4i> Defects, vector<Point> contour, Mat &frame)
{
	Point2f center;
	float x;
	int fingers = 0;

	// find the centorid of the hand 
	minEnclosingCircle(contour, center, x);
	circle(frame, center, 10, CV_RGB(0, 0, 255), 2, 8);

	for (int i = 0; i < Defects.size(); i++) {

		//extracting the start point and the depth from the Defects
		Point ptStart(contour[Defects[i].val[0]]);
		double depth = static_cast<double>(Defects[i].val[3]) / 256;
		//display start points
		circle(frame, ptStart, 5, CV_RGB(255, 0, 0), 2, 8);

		//if the depth > 11 and the start point is higher than the center, count that as a finger
		if (depth>11 && ptStart.y<center.y) {
			circle(frame, ptStart, 4, CV_RGB(255, 0, 0), 4);
			fingers++;
		}
	}

	//index: if number if fingers detected: (0,1):Rock, (2,3):Scussor, (>3):Paper
	if (fingers >1 && fingers <= 3) {
		//putText(frame, "Scissor", Point(50, 50), 2, 2, CV_RGB(0, 255, 0), 4, 8);
	}
	else if (fingers <= 1) {
		//putText(frame, "BLACK POWER", Point(50, 50), 2, 2, CV_RGB(0, 0, 0), 4, 8);
	}
	else if (fingers > 3) {
		//putText(frame, "STOP", Point(50, 50), 2, 2, CV_RGB(255, 0, 0), 4, 8);
	}
}
