#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#pragma once

#define DELTA_COMP_RESULT_A 2
#define DELTA_COMP_RESULT_B 1
#define DELTA_COMP_RESULT_NONE -1


class Guide{
    public:

        float lastTheta;
        float lastRho;

        float getValMultiplier = (float)1;
        float getVal2Multiplier = (float)0.0;
        
        float limitDeltaRho = 100;
        float limitDeltaTheta = 0.5;

        bool blinded = false;
        float currentDeltaRho = 0;
        float currentDeltaTheta = 0;
        cv::Vec2f currentClosest;
        bool found = false;
        bool applied = false;

        void apply(){
            if(found){
                applied = true;
                float theta2 = lastTheta * getVal2Multiplier;
                float rho2 = lastRho * getVal2Multiplier;
                lastTheta = (currentClosest[1]*getValMultiplier) + theta2;
                lastRho = (currentClosest[0]*getValMultiplier) + rho2;
            }
        }

        cv::Vec2f getVect(){
            return cv::Vec2f(lastRho, lastTheta);
        }

        void reset(){
            found = false;
            applied = false;
            currentClosest[0] = 1000;
            currentClosest[1] = 0;
        }

        void setNewClosest(cv::Vec2f newClosest){
            found = true;
            currentClosest = newClosest;
        }

        Guide(float start){
            lastTheta = start;
            lastRho = 0;
        }

    private:
        cv::Vec2f lastPolar;
};

float delta(float a, float b);
int deltaComp(float a, float b, float c, float limit);
void drawLinesFromPolar(cv::Mat *src, cv::Vec2f polarCoord, cv::Point origin, cv::Scalar color);
void writeTextOnFrame(cv::Mat *src, char *text, cv::Point pos, cv::Scalar color);
void convertVectorFromPoint(std::vector<cv::Vec2f> src, cv::Point newPoint, std::vector<cv::Vec2f> *result);

#endif