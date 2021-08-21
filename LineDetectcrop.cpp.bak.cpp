#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;
using namespace std;

Mat gray_image, cannyed_image, finish;

int main(int argc, char** argv )
{
    printf("data \n");
    Mat image;
    image = imread("C:/Users/nural/OneDrive/Desktop/linedetect/a.jpg", 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    cvtColor(image, gray_image, COLOR_RGB2GRAY);
    Canny(gray_image, cannyed_image, 100, 200);

    finish = image.clone();

    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(cannyed_image, linesP, 6, CV_PI/40, 160, 40, 25 ); // runs the actual detection
    // Draw the lines
    for( size_t i = 0; i < linesP.size(); i++ )
    {
        Vec4i l = linesP[i];
        line( image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,255,20), 3, LINE_AA);
    }

    namedWindow("Display Image", WINDOW_AUTOSIZE );
    imshow("Display Image", finish);
    waitKey(0);
    return 0; 
}