#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#define DELTA_COMP_RESULT_A 2
#define DELTA_COMP_RESULT_B 1
#define DELTA_COMP_RESULT_NONE -1


class Guide{
    public:

        float lastTheta;
        float lastRho;

        float getValMultiplier = (float)0.8;
        float getVal2Multiplier = (float)0.2;
        
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

float delta(float a, float b){
    
    a += CV_PI * 3.0;
    b += CV_PI * 3.0;

    float d = abs(a - b);

    while(d > CV_PI) d -= float(CV_PI);
    if(d > CV_PI/2) d = float(CV_PI) - d;

    return d;
}


// d < D
//                  true     false
int deltaComp(float a, float b, float c, float limit){
    
    a += (CV_PI * 3.0f);
    b += (CV_PI * 3.0f);
    c += (CV_PI * 3.0f);

    float d = abs(a - b);
    while(d > CV_PI) d -= float(CV_PI);
    if(d > CV_PI/2) d = float(CV_PI) - d;

    float D = abs(a - c);
    while(D > CV_PI) D -= float(CV_PI);
    if(D > CV_PI/2) D = float(CV_PI) - D;

    if(d <= D){
        if(d < limit) return DELTA_COMP_RESULT_A;
        else return DELTA_COMP_RESULT_NONE;
    }else{
        if(D < limit) return DELTA_COMP_RESULT_B;
        else return DELTA_COMP_RESULT_NONE;
    }
}

void drawLinesFromPolar(cv::Mat *src, cv::Vec2f polarCoord, cv::Point origin, cv::Scalar color){
    float rho = polarCoord[0];
    float theta = polarCoord[1];

    cv::Point pt1, pt2;
    double a = cos(theta), b = sin(theta);
    double x0 = a*rho, y0 = b*rho;
    pt1.x = cvRound(x0 + 1000*(-b)) + origin.x;
    pt1.y = cvRound(y0 + 1000*(a)) + origin.y;
    pt2.x = cvRound(x0 - 1000*(-b)) + origin.x;
    pt2.y = cvRound(y0 - 1000*(a)) + origin.y;

    cv::line( *src, pt1, pt2, color, 2, cv::LINE_AA);
}

void writeTextOnFrame(cv::Mat *src, char *text, cv::Point pos, cv::Scalar color){
    cv::putText(*src, text, pos, cv::FONT_HERSHEY_PLAIN, 2, color, 2, cv::LINE_AA);
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