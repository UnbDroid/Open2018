#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv ){

	/*Image sent as argument*/
	printf("%d\n", argc);

	if(argc != 2){
		cout << "Error!!" << endl;
	}

	// To avoid extra memory, creates a function that return only the desired Mat object?
	Mat M = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	// Remember that this mode reads image as BGR

	// Security to verify if image was really read
	if(!M.data){
		cout << "No image data!" << endl;
		return 0;
	};

	// Initial range for calibration
   	int LowR = 0;
	int HighR = 255;

	int LowG = 0; 
	int HighG = 255;

	int LowB = 0;
	int HighB = 255;

    namedWindow("Original", WINDOW_NORMAL );
	namedWindow("Changed", WINDOW_NORMAL);
	
	// create trackbars to make calibration easier
	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
	
	cvCreateTrackbar("LowR", "Control", &LowR, 255); //Red Value (0 - 255)
 	cvCreateTrackbar("HighR", "Control", &HighR, 255);

 	cvCreateTrackbar("LowG", "Control", &LowG, 255); //Green Value (0 - 255)
 	cvCreateTrackbar("HighG", "Control", &HighG, 255);

 	cvCreateTrackbar("LowB", "Control", &LowB, 255); //Blue Value (0 - 255)
 	cvCreateTrackbar("HighB", "Control", &HighB, 255);
	
	/*Loop that shows how the range affects the destiny image, press "ESC" to leave the loop*/
	Mat thresholded;
	while(1){
		
		inRange(M, Scalar(LowB, LowG, LowR), Scalar(HighB, HighG, HighR), thresholded);
		imshow("Changed", thresholded);
		imshow("Original", M);
		if(waitKey(30)==27)
			break;
	};

	// First, apply some filter
	medianBlur(thresholded, thresholded, 15);

	// Apply Canny to find edges
	Canny(thresholded, thresholded, 1, 100);
	imshow("Changed", thresholded);
	
	waitKey(0);

	// Idea 1: We can apply a better filter and avoid Hough, that will still make it possible to get the corners of the rectangle
	// Apply the Hough transform and see if we get good lines
	vector<Vec4i> lines;
	HoughLinesP(thresholded, lines, 1, CV_PI/180, 25, 50, 200);	
	
	cout << lines.size() << endl;
	
	vector<pair<int, int> > points;
	
	/*Creates another Mat object where we will show the lines detected by Hough Transform, this can be avoided*/
	Mat color;
	cvtColor( thresholded, color, CV_GRAY2BGR );
	
	for( size_t i = 0; i < lines.size(); i++ ){
		points.push_back(make_pair(lines[i][0], lines[i][1]));
		points.push_back(make_pair(lines[i][2], lines[i][3]));
		line( color, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    };
    
    /*Sort the points that represent the start and end points of the found lines*/
    sort(points.begin(), points.end());
    imshow("Changed", color);
    
    /*We need 3 points to do a affine transform*/
    vector<pair<int, int> > affine;
    
    /*First we find the top-left corner*/
    int last = points[0].first;
    int small = 2000, smalli = 0;
    for(int i=0; i< points.size(); i++){
    	if(abs(points[i].first - last)>20)
    		break;
    	if(points[i].second < small){
    		small = points[i].second;
    		smalli = i;
    	};	
    }
    
    affine.push_back(points[smalli]);
    
    /*Now we find the right corners and the length observed*/
    last = points[points.size()-1].first;
    int length = 0;
    int great=0, greati=0;
    small = 2000;
    smalli = 0;
    for(int i= points.size()-1; i>=0; i--){
    	if(abs(last - points[i].first) > 20)
    		break;
    	if(points[i].second > great){
    		great = points[i].second;
    		greati = i;
    	}
    	if(points[i].second < small){
    		small = points[i].second;
    		smalli = i;
    	};    		
    }
    
    affine.push_back(points[smalli]);
    affine.push_back(points[greati]);
    length = great - small;
    int length1 = affine[1].first - affine[0].first;
    
    for(int i=0;i<affine.size();i++){
    	cout << affine[i].first << " "
             << affine[i].second << endl;
    };
    
    /*Calculate the affine points, begin and end*/
    
    Point2f srcTri[3];
    Point2f dstTri[3];
    
    srcTri[0] = Point(affine[0].first, affine[0].second);
    srcTri[1] = Point(affine[1].first, affine[1].second);
    srcTri[2] = Point(affine[2].first, affine[2].second);
    
    dstTri[0] = Point(thresholded.cols - 1, thresholded.rows - length1 - 1);
    dstTri[1] = Point(thresholded.cols - 1, thresholded.rows - 1);
    dstTri[2] = Point(thresholded.cols - length -1, thresholded.rows - 1);
    
    cout << "L:" << length << " " << "L1:" << length1 << endl;
    
    /*warp_map used in the affine transform*/
    Mat warp_map(2, 3, CV_32FC1);
    
    warp_map = getAffineTransform(srcTri, dstTri);
    
    warpAffine( thresholded, thresholded, warp_map, thresholded.size());
    
    imshow("Changed", thresholded);
    
    waitKey(0);

    return 0;
}
