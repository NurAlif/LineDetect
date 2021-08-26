#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
    VideoCapture vid_capture("C:/Users/nural/Downloads/b.mp4");

    int frame_counter, frame_count;
    frame_count = cvRound(vid_capture.get(CAP_PROP_FRAME_COUNT));
    
    if (!vid_capture.isOpened())
        cout << "Error opening video" << endl;
    else{
        int fps = cvRound(vid_capture.get(5));
        cout << "Frames per second :" << fps;
        cout << "  Frame count :" << frame_count;
    }

    vid_capture.set(CAP_PROP_POS_FRAMES, 100);

    Mat src, canny;
    while(vid_capture.isOpened()){
        vid_capture.read(src);
        

        if(src.empty()){
            vid_capture.set(CAP_PROP_POS_FRAMES, 100);
            return EXIT_FAILURE;
        }

        Mat gray;
        cvtColor(src, gray, COLOR_BGR2GRAY);
        // medianBlur(gray, gray, 5);


        cv::Canny(gray, canny, 200,20);
        cv::namedWindow("canny2"); cv::imshow("canny2", canny>0);

        std::vector<cv::Vec3f> circles;

        /// Apply the Hough Transform to find the circles
        cv::HoughCircles( gray, circles, HOUGH_GRADIENT, 1, 60, 200, 20, 0, 0 );



        for( size_t i = 0; i < circles.size(); i++ )
        {
            Vec3i c = circles[i];
            Point center = Point(c[0], c[1]);
            // circle center
            circle( src, center, 1, Scalar(0,100,100), 3, LINE_AA);
            // circle outline
            int radius = c[2];
            circle( src, center, radius, Scalar(255,0,255), 3, LINE_AA);
        }



        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", gray);
        int key = waitKey(10);
        if (key == 'q')
        {
            cout << "q" << endl;
            break;
        }
        if (key == ' ')
        {
            cout << "pause : " << frame_counter << endl;
            while(true){
                int key2 = waitKey(60);
                if(key2 == ' ') break;
            }
        }
    }

    vid_capture.release();
    destroyAllWindows();
    return 0;
}