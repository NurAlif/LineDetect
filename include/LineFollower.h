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

            origin = cv::Point(cvRound(frame_width/2), frame_height);
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

                    if(theta > biggestTheta && theta > -0.5){
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

                        if(theta > biggestTheta && theta > -0.5){
                            A.setNewClosest(vectLines[i]);
                            biggestTheta = theta;
                            continue;
                        }
                            
                    }
                    
                    A.apply();
                    B.apply();

                    drawLinesFromPolar(&mat_finish, A.getVect(), origin, cv::Scalar(0,255,255));
                    drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(0,0,255));

                    if(B.lastRho < appr2MinRhoTrigger){
                        phase = PHASE_APPROACH_FINAL;
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

                    std::vector<cv::Vec2f> clusterB;

                    float biggestTheta = -2;
                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];

                        float rho0 = lines[i][0];
                        float theta0 = rho0 < 0? -lines[i][1] : lines[i][1];
                        rho0 = abs(rho0);

                        if(theta0 < thetaPoleMax && theta0 > thetaPoleMin){
                            pole.setNewClosest(vectLines[i]);
                            continue;
                        }

                        if(theta < 0 && theta > -1.4 && theta > biggestTheta){
                            biggestTheta = theta;
                            A.setNewClosest(vectLines[i]);
                        }

                        if( delta(theta, B.lastTheta) < 0.25f){
                            B.setNewClosest(vectLines[i]);
                            // clusterB.push_back(vectLines[i]);
                        }
                    }

                    B.apply();
                    if(delta(B.lastTheta, A.currentClosest[1]) > deltaABMin) A.apply();
                    // if(B.found){
                    //     if(delta(B.lastTheta, A.currentClosest[1]) > deltaABMin){
                    //     }
                    //     pole.apply();
                    // }
                    
                    if(delta(B.lastTheta, pole.currentClosest[1]) > 0.1) 
                    pole.apply();


                    if(B.found) drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(0,0,255));
                    if(A.applied) {
                        drawLinesFromPolar(&mat_finish, A.getVect(), origin, cv::Scalar(0,255,0));
                    }
                    if(pole.found)drawLinesFromPolar(&mat_finish, pole.getVect(), origin, cv::Scalar(255,255,0));

                    if(B.lastRho < turnMinRhoTrigger){
                        std::cout << "triggered" << std::endl;
                        turnFinished = true;
                        waitPassTurnStart = now;
                    }
                }
                break;
            case PHASE_RETURN:
                {
                    A.reset();

                    size_t lenLine = vectLines.size();

                    float biggestTheta = -2;
                    for(size_t i = 0; i < lenLine; i++){
                        float rho = vectLines[i][0];
                        float theta = vectLines[i][1];

                        if(delta(theta, A.lastTheta) < 0.4 && rho < A.currentClosest[0]){
                            A.setNewClosest(vectLines[i]);
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