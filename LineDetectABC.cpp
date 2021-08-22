#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "Utils.h"

#define PHASE_TURN_AROUND = 1
#define PHASE_APPROACH = 0
#define PHASE_RETURN = 2

using namespace cv;
using namespace std;

Mat gray_image, cannyed_image, finish;

Guide A(0);
Guide B(1.57f);

int phase = 0;

int main(int argc, char** argv )
{
    VideoCapture vid_capture("C:/Users/nural/OneDrive/Desktop/linedetect/a.mp4");

    if (!vid_capture.isOpened())
        cout << "Error opening video stream or file" << endl;
    else{
        int fps = vid_capture.get(5);
        cout << "Frames per second :" << fps;
        int frame_count = vid_capture.get(CAP_PROP_FRAME_COUNT);
        cout << "  Frame count :" << frame_count;
    }

    int offsetStart = 10;

    Mat image;
    while (vid_capture.isOpened())
    {
        vid_capture.read(image);
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }

        if(offsetStart > 0){
            offsetStart--;
            continue;
        }

        rotate(image, image, ROTATE_90_COUNTERCLOCKWISE);
        
        Rect myROI(5, 140, image.cols-10, image.rows-280);
        Mat croppedImage = image(myROI);

        cvtColor(croppedImage, gray_image, COLOR_RGB2GRAY);
        Canny(gray_image, cannyed_image, 10, 200);
        finish = croppedImage.clone();


        vector<Vec2f> lines; // will hold the results of the detection
        HoughLines(cannyed_image, lines, 1, CV_PI/180, 150, 0, 0 ); // runs the actual detection
        // Draw the lines
        
        Vec4f nearestA(A.lastAngle, A.lastAngle, A.lastAngle,10);
        Vec4f nearestB(B.lastAngle, B.lastAngle, B.lastAngle,10);
        
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));


            float d = delta(nearestA[2], theta);
            if(d < nearestA[3]){
                nearestA[3] = d;
                nearestA[1] = theta;
                nearestA[0] = rho;
            }

            d = delta(nearestB[2], theta);
            if(d < nearestB[3]){
                nearestB[3] = d;
                nearestB[1] = theta;
                nearestB[0] = rho;
            }

            line( finish, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
        }

        if(nearestA[3] < 0.5f){
            A.lastAngle = nearestA[1];

            float rho = nearestA[0], theta = nearestA[1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( finish, pt1, pt2, Scalar(255,0,0), 3, LINE_AA);
        }

        if(nearestB[3] < 0.5f){
            B.lastAngle = nearestB[1];

            float rho = nearestB[0], theta = nearestB[1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( finish, pt1, pt2, Scalar(55,255,0), 3, LINE_AA);
        }

        // display
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", finish);
        int key = waitKey(20);
        if (key == 'q')
        {
            cout << "q key is pressed by the user. Stopping the video" << endl;
            break;
        }
        if (key == ' ')
        {
            cout << "pause" << endl;
            while(true){
                int key2 = waitKey(60);
                if(key2 == ' ') break;
            }
        }

    }
    vid_capture.release();
    destroyAllWindows();

    return 0; 
}