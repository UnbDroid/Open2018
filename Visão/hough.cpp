#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int lowThreshold = 40;
int const max_lowThreshold = 100;
int i = 2;
int rho = 1;
int threshold2 = 50;
int minLineLength = 500;
int maxLineGap = 100;


int main(int argc, char** argv)
{
  const char* filename = argc >= 2 ? argv[1] : "filesTest/video1.mp4";

  VideoCapture cap(filename);
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    return -1;
  }
  Mat src;
  /*
  Mat src = imread(filename, 0);
  if(src.empty())
  {
     cout << "can not open " << filename << endl;
     return -1;
  }
  */
  Mat dst, cdst, blur;

  namedWindow("Trackbar", CV_WINDOW_AUTOSIZE );
  createTrackbar( "Min Threshold:", "Trackbar", &lowThreshold, max_lowThreshold);
  createTrackbar( "Blur:", "Trackbar", &i, 20);
  createTrackbar( "rho:", "Trackbar", &rho, 100);
  createTrackbar( "Threshold:", "Trackbar", &threshold2, 100);
  createTrackbar( "MinLineLength:", "Trackbar", &minLineLength, 1000);
  createTrackbar( "MaxLineGap:", "Trackbar", &maxLineGap, 1000);


  while (1){
    //printf ("0\n");
    cap >> src;
    if (src.empty())
       break;

    imshow("Original", src);
    cvtColor(src, src, CV_RGB2GRAY);
    resize(src, src, cv::Size(), 0.6, 0.6, INTER_LINEAR);
    GaussianBlur( src, blur, Size( i*2+1, i*2+1 ), 0, 0 );
    imshow("Blur", blur);

//printf ("1\n");
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y, grad;
    Mat src_gray;
    int scale = 1, delta = 0, ddepth = CV_16S;
    Sobel( blur, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    Sobel( blur, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    convertScaleAbs( grad_y, abs_grad_y );
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    imshow("Sobel", grad);

    Canny(blur, dst, lowThreshold, lowThreshold*3, 3);
    imshow("Canny", dst);

    //Hough para Canny
    cvtColor(dst, cdst, CV_GRAY2BGR);
    vector<Vec4i> lines;
    HoughLinesP(dst, lines, 1, CV_PI/180, threshold2, double (minLineLength/10), double (maxLineGap/10));
    for( size_t i = 0; i < lines.size(); i++ )
    {
      Vec4i l = lines[i];
      line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
    }
    imshow("Hough", cdst);

    //printf ("3\n");
    //Hough para Sobel
    Mat cdst2;
    cvtColor(grad, cdst2, CV_GRAY2BGR);

    //printf ("3.1\n");
    vector<Vec4i> lines2;
    //HoughLinesP(grad, lines2, 1, CV_PI/180, 50, 50, 10);
    HoughLinesP(grad, lines2, 1, CV_PI/180, threshold2, double (minLineLength/10), double (maxLineGap/10));

    //printf ("3.2\n");
    for( size_t i = 0; i < lines.size(); i++ )
    {
      Vec4i l = lines[i];
      line(cdst2, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
    }
    imshow("Hough2", cdst2);

//printf ("4\n");
    if (waitKey(100)>0){
        break;
    }
  }
  return 0;
}
