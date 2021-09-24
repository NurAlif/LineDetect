#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "BlueRect.h"

using namespace cv;
using namespace std;

int lowH1 = 28;       // Set Hue
int highH1 = 50;

int lowS1 = 100;       // Set Saturation
int highS1 = 255;

int lowV1 = 0;       // Set Value
int highV1 = 255;	

const int FRAME_WIDTH = 320;
const int FRAME_HEIGHT = 240;

int main(int argc, char** argv )
{

    
    

    cv::namedWindow("Setting Bola", CV_WINDOW_AUTOSIZE);

    cv::createTrackbar("LowH", "Setting Bola", &lowH1, 179); //Hue (0 - 179)
    cv::createTrackbar("HighH", "Setting Bola", &highH1, 179);

    cv::createTrackbar("LowS", "Setting Bola", &lowS1, 255); //Saturation (0 - 255)
    cv::createTrackbar("HighS", "Setting Bola", &highS1, 255);

    cv::createTrackbar("LowL", "Setting Bola", &lowV1, 255); //Value (0 - 255)
    cv::createTrackbar("HighL", "Setting Bola", &highV1, 255);



    VideoCapture vid_capture(1);
    vid_capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
    vid_capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
    
    if (!vid_capture.isOpened())
        cout << "Error opening video" << endl;
    else{
        int fps = cvRound(vid_capture.get(5));
    }

    int offsetStart = 30;
    vid_capture.set(CAP_PROP_POS_FRAMES, offsetStart);

    BlueRect br = BlueRect(&vid_capture);
    br.SrcHSV(&lowH1, &highH1, &lowS1, &highS1, &lowV1, &highV1);

    Mat image;
    while (vid_capture.isOpened()){

        vid_capture.read(image);
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }

        // process
        br.processImage(&image);

        /*
        double pi = 3.14;
        std::stringstream stream;
        stream << std::fixed << std::setprecision(4) << pi;
        cv::putText(lf.getfinishMat(), stream.str(), Point(3,12), cv::FONT_HERSHEY_PLAIN, 1, Scalar(255,50,0));
        stream.str("");
        stream << std::fixed << std::setprecision(4) << 90.8229745;
        cv::putText(lf.getfinishMat(), stream.str(), Point(3,24), cv::FONT_HERSHEY_PLAIN, 1, Scalar(255,50,0));
        cv::putText(lf.getfinishMat(), "text: 202.20", Point(3,36), cv::FONT_HERSHEY_PLAIN, 1, Scalar(255,50,0));
        cv::putText(lf.getfinishMat(), "text: 202.20", Point(3,48), cv::FONT_HERSHEY_PLAIN, 1, Scalar(255,50,0));
        cv::putText(lf.getfinishMat(), "text: 202.20", Point(3,60), cv::FONT_HERSHEY_PLAIN, 1, Scalar(255,50,0));
        */


        // display
        namedWindow("C1", WINDOW_AUTOSIZE );
        namedWindow("C2", WINDOW_AUTOSIZE );
        namedWindow("C3", WINDOW_AUTOSIZE );
        namedWindow("Result", WINDOW_AUTOSIZE );

        imshow("C1", br.mat_hue1);
        imshow("C2", br.mat_hue2);
        imshow("C3", br.mat_hue3);
        imshow("Finish", br.mat_finish);
        int key = waitKey(20);
        if (key == 'q')
        {
            cout << "q" << endl;
            break;
        }

    }
    vid_capture.release();
    destroyAllWindows();

    return 0; 
}


/*
47-137

390-489

*/