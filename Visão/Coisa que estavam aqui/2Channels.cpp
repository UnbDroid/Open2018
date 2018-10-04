#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <bitset>

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
	
	Mat result(M.rows, M.cols, CV_8UC1, Scalar(0));
	Mat test(M.rows, M.cols, CV_8UC1, Scalar(0));
	
	int i, j;
	for(i=0;i<M.rows;i++){
		unsigned char* m_ptr = M.ptr<uchar>(i);
		unsigned char* r_ptr = result.ptr<uchar>(i);
		unsigned char* t_ptr = test.ptr<uchar>(i);
		for(j=0;j<M.cols;j++){
			// Black and White will go as 0
			if(m_ptr[3*j]+m_ptr[3*j+1]+m_ptr[3*j+2] >= 580 || m_ptr[3*j]+m_ptr[3*j+1]+m_ptr[3*j+2] <= 140){
				r_ptr[j] = 0;
			}
			else {
				r_ptr[j] = m_ptr[3*j+2] & (7 << 5); // Red Mask
				
				short temp = m_ptr[3*j] & (15 << 4); // Blue Mask
				temp = temp >> 3;
				// Lembrar de setar o ultimo bit
				r_ptr[j] |= temp;
			
			if(r_ptr[j] < (3<<5)){
				if((r_ptr[j] & (31)) < 13)
					t_ptr[j] = 255;
				else
					t_ptr[j] = 0;	
			}
			else
				t_ptr[j] = 0;	
				
			};		
		};
	};

	namedWindow("try1", WINDOW_NORMAL );
    imshow("try1", test);
	
	while(waitKey(0)!=27);

    return 0;
}
