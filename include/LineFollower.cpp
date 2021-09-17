#include "LineFollower.h"

void LineFollower::prosessLines(){
            
    unsigned long long now = std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    switch (phase)
    {

    case PHASE_READY:
        {
            startApprTime = now;
            phase = PHASE_APPROACH;
        }break;
    case PHASE_APPROACH:
        {
            unsigned long long time_diff = now - startApprTime;

            std::stringstream stream;
            stream << std::fixed << time_diff;
            cv::putText(mat_finish, stream.str(), cv::Point(3,12), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255,255,255));

            if(time_diff >= apprTimeout){
                std::cout << "apprTimeout" << std::endl;
                phase = PHASE_APPROACH_END;
            }

        }break;
    case PHASE_APPROACH_END:
        {
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

            // drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(255,0,255));
            if(abs(B.currentClosest[0] - B.lastRho) < 30)
                drawLinesFromPolar(&mat_finish, B.getVect(), origin, cv::Scalar(0,0,255));
                B.apply();

            if(B.applied && B.lastRho < apprMinRhoTrigger){
                std::cout << "approach1finished" << std::endl;

                // approach1finished = true;
                // waitPassMidStart = now;
            }

        }break;
    case PHASE_APPROACH_FINAL:
        {
            
        }
        break;
    case PHASE_TURN_AROUND:
        {

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