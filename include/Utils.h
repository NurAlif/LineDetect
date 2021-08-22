#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

class Guide{
    public:
        bool start = false;
        int lostCount = 0;
        float startAngle = 0;
        float lastAngle = 0;
        int rho = 0;

        Guide(float start){
            lastAngle = start;
        }

    private:
};

float delta(float a, float b){
    
    a += CV_PI * 3.0f;
    b += CV_PI * 3.0f;

    float d = abs(a - b);

    while(d > CV_PI) d -= float(CV_PI);
    if(d > CV_PI/2) d = float(CV_PI) - d;

    return d;
}

float deltaDir(float a, float b){
    
    a += CV_PI * 3.0f;
    b += CV_PI * 3.0f;

    float d = abs(a - b);

    while(d > CV_PI) d -= float(CV_PI);
    if(d > CV_PI/2) d = float(CV_PI) - d;

    return d;
}