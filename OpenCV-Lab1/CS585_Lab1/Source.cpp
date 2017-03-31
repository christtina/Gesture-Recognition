/**
CS585_Lab1.cpp
@author: Ajjen Joshi
@version: 1.0 9/2/2014

used for:
CS440 Lab: Introduction to OpenCV

--------------
This program introduces the following concepts:
a) Reading an image from a file, displaying the image, writing an image to a file
b) Image representation in OpenCV: BGR and grayscale
c) Basic thresholding
--------------
*/


//The #include directive tells the preprocessor to treat the contents of a specified file as if they appear in the source program at the point where the directive appears

//core library: a compact module defining basic data structures, including the dense multi-dimensional array Mat 
//	and basic functions used by all other modules.
#include <opencv2/core/core.hpp>

//imgcodecs library: basics of reading/writing images
#include <opencv2/imgcodecs.hpp>

//image processing library:an image processing module that includes linear and non-linear image filtering,
//	geometrical image transformations (resize, affine and perspective warping, generic table-based remapping), 
//	color space conversion, histograms, and so on
#include "opencv2/imgproc/imgproc.hpp"

//highgui library: an easy-to-use interface to video capturing, image and video codecs, as well as simple UI capabilities.
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <string>

using namespace cv;
using namespace std;

// These are the function signatures (declarations) for some functions we will use 
// in today's lab. The function bodies (definitions) are underneath the 
// main() function. In C++, functions must be declared or defined in the file before you attempt to use them.
// To help keep your file organized, you can provide a declaration (just the function signature) 
// before you provide the body of the function. The arguments and return value of the declaration 
// and definition must match exactly.

/**
Creates a grayscale image from a color image.

@param src The source color image
@param dst The destination grayscale image
*/
void myGrayScale(Mat& src, Mat& dst);

/**
Creates a tinted image from a color image.

@param src The source color image
@param dst The destination tinted image
@param channel The channel specifies the tint
*/
void myTintImage(Mat& src, Mat& dst, int channel);

/**
Creates a thresholded image from a grayscale image.

@param src The source color image
@param dst The destination tinted image
@param threshold The specified threshold intensity
*/
void myThresholdImage(Mat& src, Mat& dst, int threshold);

int main()
{
	//----------------
	//a) Reading an image from a file, displaying the image, writing an image to a file
	//----------------

	//Create a Mat object that will store the data of the loaded image.
	//For more information on the Mat object: http://docs.opencv.org/doc/tutorials/core/mat_the_basic_image_container/mat_the_basic_image_container.html
	Mat image;
	//For more information on operations with images: http://docs.opencv.org/doc/user_guide/ug_mat.html

	//The imread function loads an image from a file
	//Link for documentation: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html?highlight=imread#imread
	image = imread("boston.jpg", CV_LOAD_IMAGE_COLOR);

	//cout is the command to print things to the command line in C++
	//endl inserts an end of line character / line break
	cout << "ORIGINAL IMAGE" << endl;
	cout << "no_rows: " << image.rows << endl; //print number of rows
	cout << "no_cols: " << image.cols << endl; //print number of cols
	cout << "no_channels: " << image.channels() << endl; //print numbers of channels
	cout << "image type: " << image.type() << endl; //print image type

	//Click here to see the mapping of Type to Numbers in OpenCV: http://ninghang.blogspot.com/2012/11/list-of-mat-type-in-opencv.html
	Vec3b intensity = image.at<Vec3b>(100, 100);
	cout << "intensity: " << intensity << endl; //print intensity values
	cout << "---------------" << endl << endl;

	//namedWindow is the OpenCV function for creating a new window
	//Link for documentation: http://docs.opencv.org/modules/highgui/doc/user_interface.html?highlight=namedwindow#namedwindow
	namedWindow("Image View Original", 1);

	//imshow is the OpenCV command for display an image in a window. 
	//The arguments are the name of the window, used with the namedWindow() function, 
	//and the image data, in a Mat structure
	imshow("Image View Original", image);

	if (image.empty())                      // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return 0;
	}

	//----------------
	//b) Image representation in OpenCV: BGR and grayscale
	//----------------

	//Create a Mat object that will store the data of the grayscale image
	Mat gray_image = Mat::zeros(image.rows, image.cols, CV_8UC1);
	//Call the myGreyScale function
	myGrayScale(image, gray_image);
	//Create a new window and display your grayscale image
	namedWindow("Image View Grayscale", 1);
	imshow("Image View Grayscale", gray_image);
	cout << "GRAYSCALE IMAGE" << endl;
	cout << "no_rows: " << gray_image.rows << endl;
	cout << "no_cols: " << gray_image.cols << endl;
	cout << "no_channels: " << gray_image.channels() << endl;
	cout << "image type: " << gray_image.type() << endl;

	int gray_intensity = gray_image.at<uchar>(100, 100);
	cout << "intensity: " << gray_intensity << endl;
	cout << "---------------" << endl << endl;

	//imwrite is the OpenCV command to write an image to a file.
	//The arguments are the filename and the image Mat object
	imwrite("grayscale_image.jpg", gray_image);

	
	//Create a Mat object that will store the data of the tinted image
	Mat tint_image = Mat::zeros(image.rows, image.cols, CV_8UC1);
	//Call the myTintImage function
	int chan = 2; //0: BLUE channel, 1: GREEN channel, 2: RED channel
	myTintImage(image, tint_image, chan);
	//Create a new window and display your tinted image
	namedWindow("Image View Tint", 1);
	imshow("Image View Tint", tint_image);
	imwrite("tint_image.jpg", tint_image);
	
	//----------------
	//c) Basic thresholding
	//----------------
	//Create a Mat object that will store the data of the thresholded image
	Mat thres_image = Mat::zeros(image.rows, image.cols, CV_8UC1);
	int thres = 128;
	//Call the threshold
	myThresholdImage(gray_image, thres_image, thres);
	//Create a new window and display your thresholded image
	namedWindow("Image View Thres", 1);
	imshow("Image View Thres", thres_image);
	imwrite("threshold_image.jpg", thres_image);

	//A call to waitKey() starts a message passing cycle that waits for a key stroke in the "image" window
	char key = waitKey(0);

	//Using cout, different types of output can be chained together using the << operator
	cout << "You pressed " << key << "." << endl;

	//in C++, main returns an integer, which is usually 0 by convention
	return 0;
}

//Creates a grayscale image from a color image.
void myGrayScale(Mat& src, Mat& dst) {
	//Different algorithms for converting color to grayscale: http://www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale/
	dst = Mat::zeros(src.rows, src.cols, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
	for (int i = 0; i < src.rows; i++){
		for (int j = 0; j < src.cols; j++){
			//For each pixel, compute the average intensity of the 3 color channels
			Vec3b intensity = src.at<Vec3b>(i,j); //Vec3b is a vector of 3 uchar (unsigned character)
			int avg = (intensity[0] + intensity[1] + intensity[2])/3;
			dst.at<uchar>(i,j) = avg;
		}
	} 
	cvtColor(src, dst, CV_BGR2GRAY); //cvtColor documentation: http://docs.opencv.org/modules/imgproc/doc/miscellaneous_transformations.html
}

//Creates a tinted image from a color image.
void myTintImage(Mat& src, Mat& dst, int channel) {
	dst = src.clone(); //the clone methods creates a deep copy of the matrix
	for (int i = 0; i < src.rows; i++){
		for (int j = 0; j < src.cols; j++){
		//For each pixel, suppress the channels other than that passed in the argument of the function
			dst.at<Vec3b>(i,j)[(channel + 1)%3] = 0;
			dst.at<Vec3b>(i,j)[(channel + 2)%3] = 0;
		}
	}
}

//Creates a thresholded image from a grayscale image.
void myThresholdImage(Mat& src, Mat& dst, int threshold) {
	dst = src.clone();
	for (int i = 0; i < src.rows; i++){
		for (int j = 0; j < src.cols; j++){
			//For each pixel, assign intensity value of 0 if below threshold, else assign intensity value of 255
			int intensity = src.at<uchar>(i,j);
			if (intensity < threshold) {
				dst.at<uchar>(i,j) = 0;
			}
			else {
				dst.at<uchar>(i,j) = 255;
			}
		}
	}
}

//Other useful links:
//Passing arguments to C++ functions
//	by value: http://www.learncpp.com/cpp-tutorial/72-passing-arguments-by-value/
//  by reference: http://www.learncpp.com/cpp-tutorial/73-passing-arguments-by-reference/
//	by address: http://www.learncpp.com/cpp-tutorial/74-passing-arguments-by-address/
