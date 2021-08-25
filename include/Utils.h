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
    
    a += CV_PI * 3.0;
    b += CV_PI * 3.0;

    float d = abs(a - b);

    while(d > CV_PI) d -= float(CV_PI);
    if(d > CV_PI/2) d = float(CV_PI) - d;

    return d;
}

void convertVectorFromPoint(std::vector<cv::Vec2f> src, cv::Point newPoint, std::vector<cv::Vec2f> *result){
    
    for( size_t i = 0; i < src.size(); i++ ){
        float rho = src[i][0];
        float theta = src[i][1];

        float x0 = rho * std::cosf(theta);
        float y0 = rho * std::sinf(theta);

        float m = y0 < 0.00001 && y0 > -0.00001? 99.9 : -x0/y0;
        float c = y0 - (m*x0);

        float m1 = x0 < 0.00001 && x0 > -0.00001? 99.9 : y0/x0;
        float c1 = newPoint.y - (m1*newPoint.x);

        std::cout << " = " << m << " " << m1 << std::endl;

        float b = m-m1;
        
        float x = (c1-c)/b; 
        float y = ((m1*c)-(m*c1))/b;
        
        if(b < 0.00001 && b > -0.00001) {
            x = 99.0;
            y = 99.0;
        }
        

        float dx = x - newPoint.x;
        float dy = -y - newPoint.y;


        cv::Vec2f newLine(sqrtf(dx*dx + dy*dy), atan2f(dy, dx));
        result->push_back(newLine);
    }
}