#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <stdarg.h>
#include <iostream>

using namespace cv;
using namespace std;

int threshold_binR = 136;
int threshold_binG = 119;
int threshold_binB = 139;
int threshold_binL = 30;
int blockSize = 5;
int morph_elem = 0;
int morph_size = 0;

int threshold_canny = 40;
int const max_threshold_canny = 100;
int i = 2;
int rho = 1.5;
int threshold_hough = 6;
int minLineLength = 150;
int maxLineGap = 40;

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
  Mat dst, cdst, blur, src_gray;

  namedWindow("Trackbar", CV_WINDOW_AUTOSIZE );
  createTrackbar( "Th_R:", "Trackbar", &threshold_binR, 255);
  createTrackbar( "Th_G:", "Trackbar", &threshold_binG, 255);
  createTrackbar( "Th_B:", "Trackbar", &threshold_binB, 255);
  createTrackbar( "Th_L:", "Trackbar", &threshold_binL, 255);

  createTrackbar( "Th_Canny:", "Trackbar", &threshold_canny, max_threshold_canny);
  createTrackbar( "Th_Hough:", "Trackbar", &threshold_hough, 100);
  createTrackbar( "MinLineLength_Hough:", "Trackbar", &minLineLength, 10*original.cols);
  createTrackbar( "MaxLineGap_Hough:", "Trackbar", &maxLineGap, 10*original.cols);

  createTrackbar( "blockSize:", "Trackbar", &blockSize, 20);
  createTrackbar( "Blur:", "Trackbar", &i, 20);
  createTrackbar( "Element:\n 0: Rect - 1: Cross - 2: Ellipse", "Trackbar", &morph_elem, max_elem);
  createTrackbar( "Kernel size:\n 2n +1", "Trackbar", &morph_size, max_kernel_size);

  Mat element;

  while (1)
  {
    element = getStructuringElement(morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
    cap >> original;
    if (original.empty())
    {
      cap.set(CAP_PROP_POS_AVI_RATIO , 0);
      cap >> original;
    }
    imshow ("original", original);

    cvtColor(original, src, CV_BGR2Lab);
    vector<Mat> channels2;
    resize(src, src, cv::Size(), 0.5, 0.5, INTER_LINEAR);
    cvtColor(src, src_gray, CV_BGR2GRAY);
    GaussianBlur( src, blur, Size( i*2+1, i*2+1 ), 0, 0 );
    split(blur,channels2);
    //GaussianBlur(channels2[1], channels2[1], Size( i*2+1, i*2+1 ), 0, 0 );
    //GaussianBlur(channels2[2], channels2[2], Size( i*2+1, i*2+1 ), 0, 0 );

    Mat R, G, B, L;

    adaptiveThreshold(src_gray, L, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, blockSize*2+1, 2);
    dilate(L, L, element);


    //Binarização canal L
    threshold(channels2[0], channels2[0], threshold_binL, 255, 0);//L
    //Binarização canal B para retirar o azul
    threshold(channels2[2], channels2[2], threshold_binB, 255, 1);//B

    //Correção buracos confundidos com azul ou verde
    bitwise_and(channels2[0],channels2[2], channels2[2]);
    bitwise_and(channels2[0],channels2[1], channels2[1]);

    //Binarização canal A para retirar o vermelho
    threshold(channels2[1], channels2[0], threshold_binR, 255, 0);//R
    //Binarização canal A para retirar o Verde
    threshold(channels2[1], channels2[1], threshold_binG, 255, 1);

    Mat aux1, aux2, res;

    //AND (VERDE, !AZUL) -> retira coincidencias de azul do canal verde
    bitwise_not(channels2[2], aux1);
    bitwise_and(channels2[1], aux1, channels2[1]);

    //Soma de todos os canais
    bitwise_or(channels2[2], channels2[1], aux1);
    bitwise_or(aux1, channels2[0], aux1);

    //AND do adaptiveThreshold da imagem cinza, para separar divisão de conteiners da mesma cor
    bitwise_and(L, channels2[1], channels2[1]);

    //Merge dos canais
    merge(channels2, aux1);
    //Apenas correção para visualização
    cvtColor (aux1, aux1, CV_RGB2BGR);

    //Variaveis do canny
    Mat resRGB, can;

    //Canny
    Canny(aux1, can, threshold_canny, threshold_canny*3, 3);

    //HOUGH
    cvtColor(can, resRGB, CV_GRAY2BGR);
    vector<Vec4i> lines3;
    HoughLinesP(can, lines3, 1, CV_PI/180, threshold_hough, double (minLineLength/10), double (maxLineGap/10));
    //Draw hough
    for( size_t i = 0; i < lines3.size(); i++ )
    {
      Vec4i l = lines3[i];
      line( resRGB, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
    }


    //Variaveis contours
    Mat binario;
    RNG rng(12345);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    erode(channels2[2], binario, element);

    findContours(binario, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    // Draw contours
    Mat drawing = Mat::zeros( can.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
    {
      Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
    }


    //Conversões para usar ShowManyImages
    cvtColor (can, can, CV_GRAY2BGR);
    cvtColor (L, L, CV_GRAY2BGR);
    cvtColor (res, res, CV_GRAY2BGR);

    ShowManyImages ("Resultados", 5, original, src, aux1, resRGB, drawing);


    char resposta= waitKey(10);
    //Pausa
    if (resposta == int ('p')){
        waitKey();
    }
    //Fecha
    if (resposta == int ('q')){
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
  else if (nArgs == 3){
      w = 3; h = 1;
      size = 300;
  }
  else if (nArgs == 4) {
      w = 2; h = 2;
      size = 300;
  }
  else if (nArgs == 5 || nArgs == 6) {
      w = 3; h = 2;
      size = 300;
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
