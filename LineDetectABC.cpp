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

Mat gray_image, cannyed_image, finish, gradient;

Guide A(-0.3f);
Guide B(1.57f);
Guide C(0);

float ABDiff = 0;

int phase = 0;

//
/*
const int max_value_H = 360/2;
const int max_value = 255;
const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";
int low_H = 34, low_S = 83, low_V = 102;
int high_H = 87, high_S = 255, high_V = 255;
static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = min(high_H-1, low_H);
    setTrackbarPos("Low H", window_detection_name, low_H);
}
static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = max(high_H, low_H+1);
    setTrackbarPos("High H", window_detection_name, high_H);
}
static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = min(high_S-1, low_S);
    setTrackbarPos("Low S", window_detection_name, low_S);
}
static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = max(high_S, low_S+1);
    setTrackbarPos("High S", window_detection_name, high_S);
}
static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = min(high_V-1, low_V);
    setTrackbarPos("Low V", window_detection_name, low_V);
}
static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = max(high_V, low_V+1);
    setTrackbarPos("High V", window_detection_name, high_V);
}
*/
//

int main(int argc, char** argv )
{

    /*

    namedWindow(window_detection_name);
    // Trackbars to set thresholds for HSV values
    createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);


    */

    VideoCapture vid_capture("C:/Users/nural/OneDrive/Desktop/linedetect/a.mp4");

    int frame_counter, frame_count;
    frame_count = cvRound(vid_capture.get(CAP_PROP_FRAME_COUNT));
    
    if (!vid_capture.isOpened())
        cout << "Error opening video" << endl;
    else{
        int fps = cvRound(vid_capture.get(5));
        cout << "Frames per second :" << fps;
        cout << "  Frame count :" << frame_count;
    }

    int offsetStart = 10;
    vid_capture.set(CAP_PROP_POS_FRAMES, offsetStart);
    frame_counter = offsetStart;

    Mat image, hsv, filtered;
    while (vid_capture.isOpened()){


        vid_capture.read(image);
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }

        // repeat
        frame_counter++;
        if (frame_counter >= frame_count-1){
            frame_counter = offsetStart;
            vid_capture.set(CAP_PROP_POS_FRAMES, offsetStart);
        }

        // start
        if(frame_counter == offsetStart+1){

        }

        rotate(image, image, ROTATE_90_COUNTERCLOCKWISE);
        
        Rect myROI(5, 140, image.cols-10, image.rows-280);
        Mat croppedImage = image(myROI);


        cvtColor(croppedImage, hsv, COLOR_RGB2HSV);
        
        vector<Mat> channels(3);
        split(hsv, channels);
        Mat hue = channels[1];

        // inRange(hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), filtered);
        

        cvtColor(croppedImage, gray_image, COLOR_RGB2GRAY);
        Canny(hue, cannyed_image, 10, 200);
        finish = croppedImage.clone();


        vector<Vec2f> lines, vectLines; 
        HoughLines(cannyed_image, lines, 1, CV_PI/180, 150, 0, 0 ); 
        
        
        Point origin(cvRound(croppedImage.cols/2),croppedImage.rows);
        convertVectorFromPoint(lines, origin, &vectLines);
        // Draw the lines
        


        //             last Rho   last Angle    delta closest    closest
        Vec6f nearestA(A.lastRho, A.lastTheta,  1000   , 10,     A.lastRho, A.lastTheta);
        Vec6f nearestB(B.lastRho, B.lastTheta,  1000   , 10,     B.lastRho, B.lastTheta);

        //clusterA vs B
        vector<Vec2f> ACluster; 
        vector<Vec2f> BCluster;

        for( size_t i = 0; i < vectLines.size(); i++ ){
            float rho = vectLines[i][0];
            float theta = vectLines[i][1];

            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b)) + origin.x;
            pt1.y = cvRound(y0 + 1000*(a)) + origin.y;
            pt2.x = cvRound(x0 - 1000*(-b)) + origin.x;
            pt2.y = cvRound(y0 - 1000*(a)) + origin.y;

            if(rho > 250) continue;

            int testComp = deltaComp(theta, A.lastTheta, B.lastTheta, 0.6);
            if(testComp > 0){
                if(testComp == DELTA_COMP_RESULT_A){
                    ACluster.push_back(vectLines[i]);
                    line( finish, pt1, pt2, Scalar(0,0,250), 2, LINE_AA);
                }else{
                    BCluster.push_back(vectLines[i]);
                    line( finish, pt1, pt2, Scalar(250,0,0), 2, LINE_AA);
                }
            }
        }

        //get closest rho
        for(size_t i = 0; i < ACluster.size(); i++){
            float rho = ACluster[i][0];

            float d = abs(rho - nearestA[0]);

            if(d > 100) continue;

            if( d < nearestA[2]){
                nearestA[4] = rho;
                nearestA[5] = ACluster[i][1];
                nearestA[2] = d;
            }
        }

        for(size_t i = 0; i < BCluster.size(); i++){
            float rho = BCluster[i][0];

            float d = abs(rho - nearestB[0]);

            if(d > 100) continue;

            if( d < nearestB[2]){
                nearestB[4] = rho;
                nearestB[5] = BCluster[i][1];
                nearestB[2] = d;
            }
        }

        A.lastTheta = nearestA[5];
        A.lastRho = nearestA[4];

        B.lastTheta = nearestB[5];
        B.lastRho = nearestB[4];




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
            cout << "pause : " << frame_counter << endl;
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