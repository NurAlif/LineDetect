#include "LineFollower.h"

void LineFollower::prosessLines(){
            
    time_t now = time(nullptr) * 1000;
    switch (phase)
    {
    case PHASE_APPROACH_1:
        {
            if(approach1finished){
                if(now - waitPassMidStart > waitPassMid)
                    phase = PHASE_APPROACH_2;
                break;
            }

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

                if(delta(theta, X[1]) > 0.15) continue;

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

            if(B.lastRho < appr1MinRhoTrigger){
                std::cout << "approach1finished" << std::endl;

                approach1finished = true;
                waitPassMidStart = now;
            }

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

                if(delta(theta, X[1]) > 0.15) continue;

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
                origin = cv::Point(20, frame_height);

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

                if( ! isXB && delta(theta, thetaLowest) < 0.15){
                    if(abs(rhoLowest - rho) > 20 && abs(rhoLowest - rho) < 120) {
                        B.setNewClosest(lowestTheta);
                        isXB = true;
                    }
                }

                if(! isYPole) pole.setNewClosest(biggestTheta);
                if( ! isYPole && delta(theta, thetaBiggest) < 0.15){
                    if(abs(rhoBiggest - rho) > 15 && abs(rhoBiggest - rho) < 150) {
                        pole.found = false;
                        isYPole = true;
                    }
                }
            }

            B.apply();
            
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