// Declaration :- Basic code taken form http://ramsrigoutham.com/2012/11/22/panorama-image-stitching-in-opencv/
#include <stdio.h>
#include <iostream>
#include "ass2.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
 
using namespace cv;
using namespace std; 
 
/** @function main */
int main( int argc, char** argv ){

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
	
	Mat a,b;
	a = input[0];

	for(int z=1;z<argc-1;z++){	
	 	b = input[z];
 		Mat gray_image1;
 		Mat gray_image2;

// Convert to Grayscale
 		cvtColor( a, gray_image1, CV_RGB2GRAY );
 		cvtColor( b, gray_image2, CV_RGB2GRAY );
 
		if( !gray_image1.data || !gray_image2.data ){ std::cout<< " --(!) Error reading images " << std::endl; return -1; }	
//		imshow("g1",gray_image1);
//		imshow("g2",gray_image2);
//		waitKey(); 
//-- Step 1: Detect the keypoints using SURF Detector
		int min_hess=400;
		std::vector< KeyPoint > keypoints_image1, keypoints_image2;
		SurfFeatureDetector detector(min_hess);
		detector.detect( gray_image1, keypoints_image1 );
		detector.detect( gray_image2, keypoints_image2 );
 
//-- Step 2: Calculate descriptors (feature vectors)
		SurfDescriptorExtractor extractor;
		Mat descriptors_image1, descriptors_image2;
		extractor.compute( gray_image1, keypoints_image1, descriptors_image1 );
		extractor.compute( gray_image2, keypoints_image2, descriptors_image2 );
 
//-- Step 3: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match( descriptors_image1, descriptors_image2, matches );
 
//-- Quick calculation of max and min distances between keypoints
		double max_dist = 0; double min_dist = 100;
		for( int i = 0; i < descriptors_image1.rows; i++ ){ 
			double dist = matches[i].distance;
			if( dist < min_dist ) min_dist = dist;
			if( dist > max_dist ) max_dist = dist;
		}
 
//-- Use only "good" matches (i.e. whose distance is less than 3*min_dist )
		std::vector< DMatch > good_matches;
 
		for( int i = 0; i < descriptors_image1.rows; i++ ){ 
			if( matches[i].distance < 3*min_dist ){ 
				good_matches.push_back( matches[i]); 
			}
		}

		std::vector< Point2f > i1;
		std::vector< Point2f > i2;
 
//-- Get the keypoints from the good matches
		for( int i = 0; i < good_matches.size(); i++ ){
			i1.push_back( keypoints_image1[ good_matches[i].queryIdx ].pt );
			i2.push_back( keypoints_image2[ good_matches[i].trainIdx ].pt );
		}
 
// Find the Homography Matrix
		Mat H12 = findHomography( i1, i2, CV_RANSAC );
		Mat H21 = H12.inv();
		H21 = H21/H21.at<double>(2,2);

		Size sz =b.size();
		vector <Point2f> corner_2, corner_1;
		corner_2.push_back(Point2f(0,0));
		corner_2.push_back(Point2f(sz.width-1,0));
		corner_2.push_back(Point2f(0,sz.height-1));
		corner_2.push_back(Point2f(sz.width-1,sz.height-1));

		perspectiveTransform(corner_2,corner_1,H21);
		float min_x = corner_1[0].x;
		float max_x = corner_1[0].x;
		float min_y = corner_1[0].y;
		float max_y = corner_1[0].y;

		for (int i=1;i<4;i++){
  		if (min_x > corner_1[i].x)
    	 	min_x = corner_1[i].x;
  		if (max_x < corner_1[i].x)
     		max_x = corner_1[i].x;
  		if (min_y > corner_1[i].y)
     		min_y = corner_1[i].y;
  		if (max_y < corner_1[i].y)
    	 	max_y = corner_1[i].y;
		}

		Size new_sz;
		new_sz.width = max_x>a.size().width ? (int)max_x+1 : a.size().width;
		if (min_x < 0)
			new_sz.width = new_sz.width - (int)min_x;
	
		new_sz.height = max_y>a.size().height ? (int)max_y+1 : a.size().height;
		if (min_y < 0)
  		new_sz.height = new_sz.height - (int)min_y;
		
		Point offset;
		offset.x = min_x>0?0:-(int)min_x;
		offset.y = min_y>0?0:-(int)min_y;
		
		Mat new_image(new_sz.height,new_sz.width,a.type(),Scalar(0,0,0));
		int channels = a.channels();
		int nRows;
		int nCols;
		uchar* pa,*pn;
		
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
		save_img(z,a);
	}
namedWindow("output",WINDOW_NORMAL);
imshow("output",a);
waitKey(0);
  return 0;
}

