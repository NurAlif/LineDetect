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
        cout << "Error opening video" << endl;
    else{
        int fps = cvRound(vid_capture.get(5));
        cout << "Frames per second :" << fps;
        int frame_count = cvRound(vid_capture.get(CAP_PROP_FRAME_COUNT));
        cout << "  Frame count :" << frame_count;
    }

    int offsetStart = 10;

    Mat image, hsv;
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


        cvtColor(croppedImage, hsv, COLOR_RGB2HSV);
        
        vector<Mat> channels(3);
        split(hsv, channels);
        Mat hue = channels[1];
        

        cvtColor(croppedImage, gray_image, COLOR_RGB2GRAY);
        Canny(hue, cannyed_image, 10, 200);
        finish = croppedImage.clone();


        vector<Vec2f> lines, vectLines; 
        HoughLines(cannyed_image, lines, 1, CV_PI/180, 150, 0, 0 ); 
        // Draw the lines
        
        Vec4f nearestA(A.lastAngle, A.lastAngle, A.lastAngle,10);
        Vec4f nearestB(B.lastAngle, B.lastAngle, B.lastAngle,10);
        /*
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

            line( finish, pt1, pt2, Scalar(0,0,255), 2, LINE_AA);
        }
        */


        Point origin(cvRound(croppedImage.cols/2),croppedImage.rows);
        convertVectorFromPoint(lines, origin, &vectLines);

        for( size_t i = 0; i < vectLines.size(); i++ )
        {
            float rho = vectLines[i][0], theta = vectLines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b)) + origin.x;
            pt1.y = cvRound(y0 + 1000*(a)) + origin.y;
            pt2.x = cvRound(x0 - 1000*(-b)) + origin.x;
            pt2.y = cvRound(y0 - 1000*(a)) + origin.y;

            line( finish, pt1, pt2, Scalar(255,0,0), 3, LINE_AA);
        }

        // display
        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", finish);
        int key = waitKey(20);
        if (key == 'q')
        {
            cout << "q" << endl;
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