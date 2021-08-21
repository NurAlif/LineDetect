#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

Mat gray_image, cannyed_image, finish;


// class Guide{
//     public:
//         commonAngle

//     private:
// };



int main(int argc, char** argv )
{

    VideoCapture vid_capture("C:/Users/nural/OneDrive/Desktop/linedetect/a.mp4");

    if (!vid_capture.isOpened())
        cout << "Error opening video stream or file" << endl;
    else{
        int fps = vid_capture.get(5);
        cout << "Frames per second :" << fps;
        int frame_count = vid_capture.get(CAP_PROP_FRAME_COUNT);
        cout << "  Frame count :" << frame_count;
    }

    Mat image;
    while (vid_capture.isOpened())
    {
        vid_capture.read(image);
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }


        rotate(image, image, ROTATE_90_COUNTERCLOCKWISE);
        
        Rect myROI(0, 140, image.cols, image.rows-280);
        Mat croppedImage = image(myROI);

        Mat h = Mat::zeros(croppedImage.rows,croppedImage.cols,CV_8UC3);
        h = Scalar(0,200,0);
        Mat res;
        res = croppedImage - h;

        Mat new_image = Mat::zeros( res.size(), res.type() );
        double alpha = 2.0; 
        int beta = -100;     //brig
        for( int y = 0; y < res.rows; y++ ) {
            for( int x = 0; x < res.cols; x++ ) {
                for( int c = 0; c < res.channels(); c++ ) {
                    new_image.at<Vec3b>(y,x)[c] =
                    saturate_cast<uchar>( alpha*res.at<Vec3b>(y,x)[c] + beta );
                }
            }
        }
        

        cvtColor(new_image, gray_image, COLOR_RGB2GRAY);


        Canny(gray_image, cannyed_image, 10, 200);

        finish = croppedImage.clone();

        //

        // vector<Vec4i> linesP; 
        // HoughLinesP(cannyed_image, linesP, 20, CV_PI/300, 10, 40, 10 );
        
        // vector<Vec4i> linesP2; 
        // HoughLinesP(cannyed_image, linesP2, 20, CV_PI/60, 160, 60, 15 );

        // vector<Vec4i> linesP3; 
        // HoughLinesP(cannyed_image, linesP3, 18, CV_PI/160, 60, 50, 15 );

        // linesP.insert(linesP.end(), linesP2.begin(), linesP2.end());
        // linesP.insert(linesP.end(), linesP3.begin(), linesP3.end());

        // for( size_t i = 0; i < linesP.size(); i++ )
        // {
        //     Vec4i l = linesP[i];
        //     line( finish, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,25,255), 3, LINE_AA);
        // }

        // 




        vector<Vec2f> lines; // will hold the results of the detection
        HoughLines(cannyed_image, lines, 1, CV_PI/180, 150, 0, 0 ); // runs the actual detection
        // Draw the lines
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( finish, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);
        }







        //////////
        // vector<Vec2f> lines;
        // float thres_theta = 0.05;
        // float thres_rho = 40;
        // vector<Vec2f> group;

        // for( size_t i = 0; i < linesP.size(); i++ )
        // {
        //     Vec4i l = linesP[i];
            
        //     float x1 = l[0];
        //     float y1 = l[1];
        //     float x2 = l[2];
        //     float y2 = l[3];

        //     float lx = x1;
        //     float ly = y1;
        //     float hx = x2; 
        //     float hy = y2;

        //     if(x1 > x2){ hx = x1; lx = x2;}
        //     if(y1 > y2){ hy = y1; ly = y2;}

        //     float dx = hx-lx;
        //     float dy = hy-ly;

        //     float rho = 
        //     float theta = atan2f(-y, x)

        //     Vec2f current()
        // }




        // display

        namedWindow("Display Image", WINDOW_AUTOSIZE );
        imshow("Display Image", finish);
        int key = waitKey(60);
        if (key == 'q')
        {
            cout << "q key is pressed by the user. Stopping the video" << endl;
            break;
        }
        if (key == ' ')
        {
            cout << "pause" << endl;
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