#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdarg.h>
#include <iostream>

using namespace cv;
using namespace std;

int lowThreshold = 40;
int const max_lowThreshold = 100;
int i = 2;
int rho = 1.5;
int threshold2 = 50;
int minLineLength = 800;
int maxLineGap = 60;
int kernel_size = 2;
int threshold_bin = 15;
int threshold_bin2 = 125;

int morph_elem = 0;
int morph_size = 0;
int morph_operator = 0;
int const max_operator = 4;
int const max_elem = 2;
int const max_kernel_size = 21;


void ShowManyImages(string title, int nArgs, ...) ;

int main(int argc, char** argv)
{
  int operation;
  const char* filename = argc >= 2 ? argv[1] : "filesTest/video1.mp4";

  VideoCapture cap(filename);
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
    return -1;
  }
  Mat src, original;
  cap >> original;
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
  createTrackbar( "Th_Canny:", "Trackbar", &lowThreshold, max_lowThreshold);
  createTrackbar( "Blur:", "Trackbar", &i, 20);
  //createTrackbar( "Rho_Hough:", "Trackbar", &rho, 100);
  createTrackbar( "Th_Hough:", "Trackbar", &threshold2, 100);
  createTrackbar( "MinLineLength_Hough:", "Trackbar", &minLineLength, 10*original.cols);
  createTrackbar( "MaxLineGap_Hough:", "Trackbar", &maxLineGap, 10*original.cols);
  createTrackbar( "Th_Bin_Sobel:", "Trackbar", &threshold_bin, 255);
  createTrackbar( "Th_Bin_Laplacian:", "Trackbar", &threshold_bin2, 255);
  createTrackbar( "Kernel_Laplacian:", "Trackbar", &kernel_size, 20);

  createTrackbar("Operator:\n 0: Opening - 1: Closing \n 2: Gradient - 3: Top Hat \n 4: Black Hat", "Trackbar", &morph_operator, max_operator);
  createTrackbar( "Element:\n 0: Rect - 1: Cross - 2: Ellipse", "Trackbar", &morph_elem, max_elem);
  createTrackbar( "Kernel size:\n 2n +1", "Trackbar", &morph_size, max_kernel_size);

  Mat element;
  while (1){
    element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
    operation = morph_operator + 2;
    //printf ("0\n");
    cap >> original;
    if (original.empty()){
      printf ("aqui\n");
      cap.set(CAP_PROP_POS_AVI_RATIO , 0);
      cap >> original;
      //printf("Fim Arquivo\n", );
      //break;
    }
    /*
    cvtColor(original, src, CV_BGR2Lab);
    vector<Mat> channels2;
    //Separa canais da imagem
    split(src,channels2);
    //Equaliza canal L
    equalizeHist(channels2[0],channels2[0]);
    equalizeHist(channels2[1],channels2[1]);
    equalizeHist(channels2[2],channels2[2]);
    //Reune canais novamente
    merge(channels2,src);
    //Converte para BGR
    cvtColor(src,src,CV_Lab2BGR);
    //imshow("Original", src);
    cvtColor(src, blur, CV_BGR2GRAY);
    Mat blur2;
    convertScaleAbs(channels2[1], blur);
    convertScaleAbs(channels2[2], blur2);
    addWeighted( blur, 0.5, blur2, 0.5, 0, blur );
    convertScaleAbs(channels2[0], blur2);
    addWeighted( blur, 0.8, blur2, 0.2, 0, blur );
    */
    cvtColor(original, src, CV_BGR2GRAY);
    resize(src, src, cv::Size(), 0.6, 0.6, INTER_LINEAR);
    GaussianBlur( src, blur, Size( i*2+1, i*2+1 ), 0, 0 );
    //imshow("Blur", blur);


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
    threshold(grad, grad, threshold_bin, 255, 0);

    //erode( grad, grad, element );
    morphologyEx(grad, grad, operation, element);
    //imshow("Sobel", grad);

    Mat lap, lap_hough;
    Laplacian( blur, lap, CV_16S, (kernel_size*2+1), 1, 0, BORDER_DEFAULT );
    convertScaleAbs (lap, lap);
    threshold(lap, lap, threshold_bin2, 255, 0);
    //erode(lap, lap, element);
    morphologyEx(lap, lap, operation, element);
    //imshow ("Laplacian", lap);

    Canny(blur, dst, lowThreshold, lowThreshold*3, 3);
    //imshow("Canny", dst);

    //Hough para Canny
    cvtColor(dst, cdst, CV_GRAY2BGR);
    vector<Vec4i> lines3;
    HoughLinesP(dst, lines3, 1, CV_PI/180, threshold2, double (minLineLength/10), double (maxLineGap/10));
    for( size_t i = 0; i < lines3.size(); i++ )
    {
      Vec4i l = lines3[i];
      line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
    }

    //Hough para Sobel
    Mat cdst2;
    cvtColor(grad, cdst2, CV_GRAY2BGR);
    vector<Vec4i> lines2;
    HoughLinesP(grad, lines2, 1, CV_PI/180, threshold2, double (minLineLength/10), double (maxLineGap/10));
    for( size_t i = 0; i < lines2.size(); i++ )
    {
      Vec4i l2 = lines2[i];
      line(cdst2, Point(l2[0], l2[1]), Point(l2[2], l2[3]), Scalar(0,255,0), 1, CV_AA);
    }

    //Hough para Laplacian
    cvtColor(lap, lap_hough, CV_GRAY2BGR);
    vector<Vec4i> linhas;
    HoughLinesP(lap, linhas, 1, CV_PI/180, threshold2, double (minLineLength/10), double (maxLineGap/10));
    for( size_t i = 0; i < linhas.size(); i++ )
    {
      Vec4i l = linhas[i];
      line(lap_hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 1, CV_AA);
    }

    cvtColor(src, src, CV_GRAY2BGR);
    cvtColor(blur, blur, CV_GRAY2BGR);
    cvtColor(lap, lap, CV_GRAY2BGR);
    cvtColor(grad, grad, CV_GRAY2BGR);
    cvtColor(dst, dst, CV_GRAY2BGR);
    ShowManyImages ("Resultados", 9, original, src, blur, grad, lap, dst, cdst2, lap_hough, cdst);
//printf ("4\n");
    if (waitKey(10)>0){
        break;
    }
  }
  return 0;
}


void ShowManyImages(string title, int nArgs, ...) {
  int size;
  int i;
  int m, n;
  int x, y;

  // w - Maximum number of images in a row
  // h - Maximum number of images in a column
  int w, h;

  // scale - How much we have to resize the image
  float scale;
  int max;

  // If the number of arguments is lesser than 0 or greater than 12
  // return without displaying
  if(nArgs <= 0) {
      printf("Number of arguments too small....\n");
      return;
  }
  else if(nArgs > 14) {
      printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
      return;
  }
  // Determine the size of the image,
  // and the number of rows/cols
  // from number of arguments
  else if (nArgs == 1) {
      w = h = 1;
      size = 300;
  }
  else if (nArgs == 2) {
      w = 2; h = 1;
      size = 300;
  }
  else if (nArgs == 3 || nArgs == 4) {
      w = 2; h = 2;
      size = 300;
  }
  else if (nArgs == 5 || nArgs == 6) {
      w = 3; h = 2;
      size = 200;
  }
  else if (nArgs == 7 || nArgs == 8) {
      w = 4; h = 2;
      size = 200;
  }
  else if (nArgs == 9) {
      w = 3; h = 3;
      size = 300;
  }
  else {
      w = 4; h = 3;
      size = 150;
  }

  // Create a new 3 channel image
  Mat DispImage = Mat::zeros(Size(100 + size*w, 60 + size*h), CV_8UC3);

  // Used to get the arguments passed
  va_list args;
  va_start(args, nArgs);

  // Loop for nArgs number of arguments
  for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {
      // Get the Pointer to the IplImage
      Mat img = va_arg(args, Mat);

      // Check whether it is NULL or not
      // If it is NULL, release the image, and return
      if(img.empty()) {
          printf("Invalid arguments");
          return;
      }

      // Find the width and height of the image
      x = img.cols;
      y = img.rows;

      // Find whether height or width is greater in order to resize the image
      max = (x > y)? x: y;

      // Find the scaling factor to resize the image
      scale = (float) ( (float) max / size );

      // Used to Align the images
      if( i % w == 0 && m!= 20) {
          m = 20;
          n+= 20 + size;
      }

      // Set the image ROI to display the current image
      // Resize the input image and copy the it to the Single Big Image
      Rect ROI(m, n, (int)( x/scale ), (int)( y/scale ));
      Mat temp; resize(img,temp, Size(ROI.width, ROI.height));
      temp.copyTo(DispImage(ROI));
    }
    // Create a new window, and show the Single Big Image
  namedWindow( title, 1 );
  imshow( title, DispImage);
  //waitKey();

  // End the number of arguments
  va_end(args);
}
