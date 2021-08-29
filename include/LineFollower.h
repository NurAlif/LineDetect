#include <stdio.h>
#include <iostream>
#include <chrono>
#include <ctime>   
// #include <sys/time.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "Utils.h"

#define PHASE_IDLE -1
#define PHASE_APPROACH_1 2
#define PHASE_APPROACH_2 3
#define PHASE_APPROACH_FINAL 4
#define PHASE_TURN_AROUND 5
#define PHASE_RETURN 6

class LineFollower{
    public:
        // guides
        Guide A = Guide(0);
        Guide B = Guide(-1.57);
        Guide pole = Guide(0);


        LineFollower(cv::VideoCapture *video){
            vcap = video;
            frame_width  = (int)vcap->get(cv::CAP_PROP_FRAME_WIDTH);
            frame_height = (int)vcap->get(cv::CAP_PROP_FRAME_HEIGHT);

            origin = cv::Point(cvRound(20), frame_height);
            resolutionTheta = (float)CV_PI/180;
        }

        void processImage(cv::Mat *rawImage){
            
            // rotate(image, image, ROTATE_90_COUNTERCLOCKWISE);
            // cv::Rect myROI(5, 140, rawImage.cols-10, image.rows-280);
            cv::Rect myROI(0, 0, rawImage->cols, rawImage->rows-2);
            mat_input = *rawImage;
            mat_input = mat_input(myROI);
            cvtColor(mat_input, mat_hsv, cv::COLOR_RGB2HSV);
            // cvtColor(croppedImage, gray_image, COLOR_RGB2GRAY);
        
            std::vector<cv::Mat> channels(3);
            split(mat_hsv, channels);
            mat_hue = channels[1];

            Canny(mat_hue, mat_cannyed, 10, 200);
            mat_finish = mat_input.clone();
 
            HoughLines(mat_cannyed, lines, 1, resolutionTheta, 150, 0, 0 ); 

            convertVectorFromPoint(lines, origin, &vectLines);

        }

        void drawLines(){
            for( size_t i = 0; i < vectLines.size(); i++ ){
                float rho = vectLines[i][0];
                float theta = vectLines[i][1];

                cv::Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b)) + origin.x;
                pt1.y = cvRound(y0 + 1000*(a)) + origin.y;
                pt2.x = cvRound(x0 - 1000*(-b)) + origin.x;
                pt2.y = cvRound(y0 - 1000*(a)) + origin.y;

                line( mat_finish, pt1, pt2, cv::Scalar(250,0,0), 2, cv::LINE_AA);
            }
        }

        void prosessLines(){
            
            time_t now = time(nullptr) * 1000;
            switch (phase)
            {
            case PHASE_APPROACH_1:
                {

                A.reset();
                B.reset();

                size_t lenLine = vectLines.size();

                float biggestTheta = -2;
                std::vector<cv::Vec2f> clusterA;
                for(size_t i = 0; i < lenLine; i++){
                    float rho = vectLines[i][0];
                    float theta = vectLines[i][1];

                    // if(theta > 0){
                    //     //pole
                    //     continue;
                    // }

                    if(delta(theta, B.lastTheta) < 0.4 && rho < B.currentClosest[0]){
                        B.setNewClosest(vectLines[i]);
                        continue;
                    }

                    if(theta > biggestTheta){
                        A.setNewClosest(vectLines[i]);
                        biggestTheta = theta;
                        continue;
                    }
                        
                }
                
                A.apply();
                B.apply();

                drawLinesFromPolar(&mat_finish, A.getVect(), origin, cv::Scalar(0,255,255));
                drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(0,0,255));

                if(B.lastRho < appr1MinRhoTrigger){
                    approach2finished = true;
                }

                // if(approach1finished){
                //     time_t now = time(nullptr);
                //     time_t mnow = now * 1000;
                // }

                }break;
            case PHASE_APPROACH_2:
                {
                    // if(approach1finished){

                    // }break;

                    A.reset();
                    B.reset();

                    size_t lenLine = vectLines.size();

                    cv::Vec2f X = cv::Vec2f(0, -2);

                    float biggestTheta = -2;
                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];

                        if(delta(theta, B.lastTheta) < 0.4 && rho < B.currentClosest[0]){
                            B.setNewClosest(vectLines[i]);
                            continue;
                        }

                        if(theta > X[1]){
                            X = vectLines[i];
                        }
                            
                    }

                    // A pair check
                    bool isXA = false;
                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];

                        if(delta(theta, X[1]) > 0.1) continue;

                        if(abs(X[0] - rho) > 2 && abs(X[0] - rho) < 130) {
                            A.setNewClosest(X);
                            isXA = true;
                            break;
                        }
                    }

                    B.apply();
                    
                    if(delta(A.currentClosest[1], B.currentClosest[1]) > 0.3)
                        A.apply();


                    drawLinesFromPolar(&mat_finish, A.getVect(), origin, cv::Scalar(0,255,255));
                    drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(0,0,255));

                    if(B.lastRho < appr2MinRhoTrigger){
                        phase = PHASE_APPROACH_FINAL;
                        origin = cv::Point(frame_width/2, frame_height);
                    }


                }break;
            case PHASE_APPROACH_FINAL:
                {
                    if(approachFinalFinished){
                        if(now-waitPassFinalStart > waitPassFinal){
                            phase = PHASE_TURN_AROUND;
                            B.lastTheta = -0.1;
                            A.lastTheta = -0.1;
                        }
                        break;
                    }

                    B.reset();

                    size_t lenLine = vectLines.size();

                    float biggestTheta = -2;
                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];

                        if(theta > 0){
                            //pole
                            continue;
                        }

                        if(delta(theta, B.lastTheta) < 0.4 && rho < B.currentClosest[0]){
                            B.setNewClosest(vectLines[i]);
                            continue;
                        }   
                    }
                    B.apply();

                    drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(0,0,255));

                    if(B.lastRho < apprFinalMinRhoTrigger){
                        std::cout << "finish" << std::endl;

                        approachFinalFinished = true;
                        waitPassFinalStart = now;
                    }
                }
                break;
            case PHASE_TURN_AROUND:
                {
                    if(turnFinished){
                        if(now-waitPassTurnStart > waitPassTurn){
                            phase = PHASE_RETURN;
                        }
                        break;
                    }


                    B.reset();
                    A.reset();
                    pole.reset();

                    size_t lenLine = vectLines.size();

                    cv::Vec2f lowestTheta = cv::Vec2f(-1, 5), 
                    biggestTheta = cv::Vec2f(-1, -3);

                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];

                        if(theta < lowestTheta[1]){
                            lowestTheta = vectLines[i];
                        }

                        if(theta > biggestTheta[1]){
                            biggestTheta = vectLines[i];
                        }
                    }

                    // find pair 
                    bool isXB = false, isYPole = false;
                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];
                        float rhoLowest = lowestTheta[0];
                        float rhoBiggest = biggestTheta[0];
                        float thetaLowest = lowestTheta[1];
                        float thetaBiggest = biggestTheta[1];

                        if( ! isXB && delta(theta, thetaLowest) > 0.15){
                            if(abs(rhoLowest - rho) > 2 && abs(rhoLowest - rho) < 130) {
                                B.setNewClosest(lowestTheta);
                                isXB = true;
                            }
                        }

                        if( ! isYPole && delta(theta, thetaBiggest) > 0.1){
                            if(abs(rhoBiggest - rho) < 2 || abs(rhoBiggest - rho) > 130) {
                                pole.setNewClosest(biggestTheta);
                                isYPole = true;
                            }
                        }
                    }

                    B.apply();
                    // if(delta(B.lastTheta, 2) > 0.1) pole.apply();
                    // if(B.found){
                    //     if(delta(B.lastTheta, A.currentClosest[1]) > deltaABMin){
                    //     }
                    //     pole.apply();
                    // }
                    
                    if(delta(B.lastTheta, pole.currentClosest[1]) > 0.1) 
                        pole.apply();


                    if(B.applied) drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(0,0,255));
                    if(pole.applied)drawLinesFromPolar(&mat_finish, pole.getVect(), origin, cv::Scalar(255,255,0));

                    if(B.lastRho < turnMinRhoTrigger){
                        // std::cout << "triggered" << std::endl;
                        // turnFinished = true;
                        // waitPassTurnStart = now;
                    }
                }
                break;
            case PHASE_RETURN:
                {
                    A.reset();

                    size_t lenLine = vectLines.size();

                    float biggestTheta = -2;
                    std::vector<cv::Vec2f> clusterA;
                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];

                        if(delta(theta, A.lastTheta) < 0.4){
                            clusterA.push_back(vectLines[i]);
                            continue;
                        }
                            
                    }

                    for(size_t i = 0; i < lenLine; i++){
                        float rho = clusterA[i][0];
                        float theta = clusterA[i][1];

                        if(rho < A.currentClosest[0]){
                            A.setNewClosest(clusterA[i]);
                            continue;
                        }
                    }
                    
                    A.apply();

                    drawLinesFromPolar(&mat_finish, A.getVect(), origin, cv::Scalar(0,255,255));

                }break;
            case PHASE_IDLE:

                break;
            
            default:
                break;
            }
            char str[200];
            sprintf(str,"  PHASE : %d", phase);

            writeTextOnFrame(&mat_finish, str, cv::Point(cvRound(frame_width/2)-90, 30), cv::Scalar(255,255,255));
        }

        cv::Mat getfinishMat(){
            return mat_finish;
        }

        void reset(){
            lines.clear();
            vectLines.clear();
        }

    private:
        float outputTurn = 0;
        int phase = 3;

        // detection
        float resolutionTheta;
        cv::VideoCapture *vcap;
        int frame_height = 0;
        int frame_width = 0;

        // approach
        float approachRhoTarget = 0;
        float approachThetaTarget = 0;

        bool approach1finished = false;
        bool approach2finished = false;
        bool approachFinalFinished = false;
        float appr1MinRhoTrigger = 30;
        float appr2MinRhoTrigger = 200;
        float apprFinalMinRhoTrigger = 30;
        time_t waitPassMid = 5;
        time_t waitPassMidStart = 0;
        time_t waitPassFinal = 2000;
        time_t waitPassFinalStart = 0;


        // turn arround
        float deltaABMin = 0.5;
        float thetaPoleMin = 0.15;
        float thetaPoleMax = 1.2;
        float turnMinRhoTrigger = 40;
        time_t waitPassTurn = 3;
        time_t waitPassTurnStart = 0;
        bool turnFinished = false;


        // prosess
        cv::Mat mat_input, mat_hsv, mat_hue, mat_cannyed, mat_finish;
        std::vector<cv::Vec2f> lines, vectLines;
        cv::Point origin;
        




        

};