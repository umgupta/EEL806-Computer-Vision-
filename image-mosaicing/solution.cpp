#include "ass2.h"
#include "params.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/calib3d/calib3d.hpp"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){

// exit if less than 2 files given 
	const char* file_name;
	if(argc < 3){
		usage( argv);
		exit(1);
	}

// read images
	Mat input[argc-1];
	for (int i = 0; i <argc-1;i++){
		file_name = argv[i+1];
		input[i] = imread(file_name, CV_LOAD_IMAGE_COLOR);
	}


// a is the reference image and b is the other image to be merged with a. 
	Mat a,b;
	a = input[0];

	for(int z=1;z<argc-1;z++){
		b = input[z];	
//just to take the in put points via mouse, so that color by circle do not stays
		Mat aclone =a;
		Mat bclone =b;
		namedWindow("image 1", WINDOW_NORMAL);
		namedWindow("image 2", WINDOW_NORMAL);
		imshow("image 1", aclone);
		imshow("image 2", bclone);
 
// get mouse input and store in point_1 and point_2 vectors
// Pressing any key will proceed you to selecting point on next image
// If you press esc after selecting the point on second image, it will exit and 
// start the transformation
   	
		int key;
  	vector <Point2f> point_1, point_2;
  	Point p;
		do{
    	setMouseCallback("image 1", onMouse, &p);
    	waitKey(0);
    	cout << "in image 1 at " <<p << std::endl;
    	point_1.push_back(p);
    	circle(aclone, p, 2, DOTCOLOR,-1,0);
    	imshow ("image 1", aclone);

    	setMouseCallback("image 2",onMouse,&p);
    	waitKey(0);
    	cout <<"in image 2 at "<< p <<std::endl;
    	point_2.push_back(p);
    	circle(bclone, p, 2, DOTCOLOR,-1,0);
    	imshow ("image 2", bclone);

    	key = waitKey(0);
  	}while(key!=27);

		Mat H12 = findHomography(point_1, point_2);
		Mat H21 = findHomography(point_2, point_1);
// In case this are illconditioned, or h comes out very close to 0, mosaic wont work.
		cout <<H21<<endl;
		cout <<H12<<endl;
		
//map the corners of b to a; for finding the size of new image
		Size sz = b.size();
		vector <Point2f> corner_b, corner_a;
		corner_b.push_back(Point2f(0,0));
		corner_b.push_back(Point2f(sz.width-1,0));
		corner_b.push_back(Point2f(0,sz.height-1));
		corner_b.push_back(Point2f(sz.width-1,sz.height-1));

		perspectiveTransform(corner_b,corner_a,H21);
		float min_x = corner_a[0].x;
		float max_x = corner_a[0].x; 
		float min_y = corner_a[0].y;
		float max_y = corner_a[0].y;
	
		for (int i=1;i<4;i++){
			if (min_x > corner_a[i].x)
				min_x = corner_a[i].x;
			if (max_x < corner_a[i].x)
				max_x = corner_a[i].x;
			if (min_y > corner_a[i].y)
				min_y = corner_a[i].y;
			if (max_y < corner_a[i].y)
				max_y = corner_a[i].y;
		}

		Size new_sz;
		new_sz.width = max_x>a.size().width ? (int)max_x+1 : a.size().width;
		if (min_x < 0)
			new_sz.width = new_sz.width - (int)min_x;
		new_sz.height = max_y>a.size().height ? (int)max_y+1 : a.size().height;
  		if (min_y < 0)
    new_sz.height = new_sz.height - (int)min_y;
		
		cout << "Minimum x is " <<max_x<<"; max x is "<<min_x<<endl;
 		cout <<"Minimum Y is"<< max_y<<"; max y is "<<min_y<<endl;	
 		cout <<"New image size " <<new_sz<<endl;

		//calculate the offset for copying image a
		Point offset;
		offset.x = min_x>0?0:-(int)min_x;
		offset.y = min_y>0?0:-(int)min_y;
		cout <<"offset is "<<offset<<endl;
		
		Mat new_image(new_sz.height,new_sz.width,a.type());
		int channels = a.channels();
		int nRows;
  	int nCols;
		uchar* pa,*pn;

		//copy image a 
		nRows = a.rows;
		nCols = a.cols;
		for (int i=0;i<nRows;i++){
			pn = new_image.ptr<uchar>(i+offset.y);
			pa = a.ptr<uchar>(i);
			for(int j=0;j<nCols;j++){	
				for(int k=0;k<channels;k++){
					pn[(j+offset.x)*channels+k] = pa[j*channels+k];
				}
			}
		}
		
		//use homography to map b to a and copy
		nRows = new_image.rows;
		nCols = new_image.cols;
		for(int i=0;i<nRows;i++){
			for(int j=0;j<nCols;j++){
				Point2f corr = get_homograph(H12,Point(j-offset.x,i-offset.y));
				if ((int)corr.x>0 && (int)corr.x<b.size().width-1 && (int)corr.y>0 && (int)corr.y<b.size().height-1){
					pa = b.ptr<uchar>((int)corr.y);
					pn = new_image.ptr<uchar>(i);
					for(int k=0;k<channels;k++){
          	pn[j*channels+k] = pa[(int)(corr.x)*channels+k];
        	}
				}
			}
		}
		a=new_image;
		//save temperory images 
		save_img(z,a);
	}
//show result	
	namedWindow("output",WINDOW_NORMAL);
	imshow("output",a);
	waitKey(0);
	return 0;
}
