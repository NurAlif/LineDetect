#ifndef BLUERECT_H
#define BLUERECT_H

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <ctime>   
// #include <sys/time.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "Utils.h"
#pragma once

class BlueRect{
    public:
        cv::Mat mat_input, mat_hsv, mat_hue, mat_cannyed, mat_finish, mat_hue1,mat_hue2,mat_hue3;
        // guides
        float output = 0;

        BlueRect(cv::VideoCapture *video){
            vcap = video;
            frame_width  = (int)vcap->get(cv::CAP_PROP_FRAME_WIDTH);
            frame_height = (int)vcap->get(cv::CAP_PROP_FRAME_HEIGHT);
        }

        void processImage(cv::Mat *rawImage){
            
            cv::Rect myROI(0, 0, rawImage->cols, rawImage->rows-2);
            mat_input = *rawImage;
            mat_input = mat_input(myROI);
            cvtColor(mat_input, mat_hsv, cv::COLOR_RGB2HSV);
        
            std::vector<cv::Mat> channels(3);
            split(mat_hsv, channels);
            mat_hue1 = channels[0];
            mat_hue2 = channels[1];
            mat_hue3 = channels[2];

            mat_finish = mat_hue1.clone();

            inRange(mat_hsv, cv::Scalar(*lowH1, *lowS1, *lowV1), cv::Scalar(*highH1, *highS1, *highV1), mat_finish);

        }

        cv::Mat getfinishMat(){
            return mat_finish;
        }

        void reset(){
        }

        void SrcHSV(int *lowH1_, int *highH1_, int *lowS1_, int *highS1_, int *lowV1_, int *highV1_){
            lowH1 = lowH1_;
            highH1 = highH1_;

            lowS1 = lowS1_;
            highS1 = highS1_;

            lowV1 = lowV1_;
            highV1 = highV1_;
        }

    private:

        int *lowH1;
        int *highH1;

        int *lowS1;
        int *highS1;

        int *lowV1;
        int *highV1;
 

        // chrono
        unsigned long long startApprTime = 0;

        // detection
        float resolutionTheta;
        cv::VideoCapture *vcap;
        int frame_height = 0;
        int frame_width = 0;

        // prosess
        cv::Point origin;  
};





#endif