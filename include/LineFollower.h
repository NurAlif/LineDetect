#include <stdio.h>
#include <iostream>
#include <chrono>
#include <ctime>   
#include <sys/time.h>
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
            mat_input = *rawImage;
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
            // loop
            switch (phase)
            {
            case PHASE_APPROACH_1:
                

                if(approach1finished){
                    time_t now = time(nullptr);
                    time_t mnow = now * 1000;
                }

                break;
            case PHASE_APPROACH_2:

                break;
            case PHASE_APPROACH_FINAL:

                break;
            case PHASE_TURN_AROUND:

                break;
            case PHASE_IDLE:

                break;
            
            default:
                break;
            }
        }

        cv::Mat getfinishMat(){
            return mat_finish;
        }

    private:
        float outputTurn = 0;
        int phase = 3;

        // detection
        float resolutionTheta;
        cv::VideoCapture *vcap;
        int frame_height = 0;
        int frame_width = 0;

        // guides
        Guide A = Guide(0);
        Guide B = Guide(0);
        Guide pole = Guide(0);

        // approach
        float approachRhoTarget = 0;
        float approachThetaTarget = 0;

        bool approach1finished = false;
        float minRhoTrigger = 30;
        time_t waitPassMid = 5;
        time_t waitPassMidStart = 0;


        // turn arround


        // prosess
        cv::Mat mat_input, mat_hsv, mat_hue, mat_cannyed, mat_finish;
        std::vector<cv::Vec2f> lines, vectLines;
        cv::Point origin;
        




        

};