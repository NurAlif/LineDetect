#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main(){

  VideoCapture cap(0); 

  if(!cap.isOpened()){
   	cout << "Error opening video stream" << endl;
        return -1;
  }
  
  int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  
  VideoWriter video("outcpp.avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, Size(frame_width,frame_height));

  while(1){

    Mat frame;
   
    // Capture frame-by-frame
    cap >> frame;
 
    // If the frame is empty, break immediately
    if (frame.empty())
      break;
    
    // Write the frame into the file 'outcpp.avi'
    video.write(frame);
   
    // Display the resulting frame    
    imshow( "Frame", frame );
 
    // Press  ESC on keyboard to  exit
    char c = (char)waitKey(1);
    if( c == 27 ) 
      break;
  }

  // When everything done, release the video capture and write object
  cap.release();
  video.release();

  // Closes all the frames
  destroyAllWindows();
  return 0;
}