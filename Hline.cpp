#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "LineFollower.h"

using namespace cv;
using namespace std;

// LineFollower()

int main(int argc, char** argv )
{

    VideoCapture vid_capture("C:/Users/nural/Downloads/last.AVI");

    int frame_counter, frame_count;
    frame_count = cvRound(vid_capture.get(CAP_PROP_FRAME_COUNT));
    
    if (!vid_capture.isOpened())
        cout << "Error opening video" << endl;
    else{
        int fps = cvRound(vid_capture.get(5));
        cout << "Frames per second :" << fps;
        cout << "  Frame count :" << frame_count;
    }

    int offsetStart = 30;
    vid_capture.set(CAP_PROP_POS_FRAMES, offsetStart);
    frame_counter = offsetStart;

    LineFollower lf = LineFollower(&vid_capture);

    Mat image;
    while (vid_capture.isOpened()){
        lf.reset();

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

        // process
        lf.processImage(&image);
        lf.drawLines();
        lf.prosessLines();

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
        namedWindow("Display Image", WINDOW_AUTOSIZE );

        imshow("Display Image", lf.getfinishMat());
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


/*
47-137

390-489

*/