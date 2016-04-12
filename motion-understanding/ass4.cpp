#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "params.h"
#include <math.h>

using namespace std;
using namespace cv; 

void usage(char* argv[]){
  cout << "usage "<<argv[0] <<" <VideoFile>" <<std::endl;  
}

void onMouse(int e, int x, int y, int d, void *ptr ){
  Point *p  = (Point* )ptr;
  p->x      = x;
  p->y      = y;
}

Mat process(Mat a, Mat b){
    Mat bwA,bwB;
    cvtColor(a,bwA,CV_BGR2GRAY);
    cvtColor(b,bwB,CV_BGR2GRAY);

    Mat it = Mat::zeros(bwA.rows,bwA.cols,CV_8U);
        it = abs(bwB - bwA);                   //I close my eyes here
    Mat ix = Mat::zeros(bwA.rows,bwA.cols,CV_8U);
    Mat iy = Mat::zeros(bwA.rows,bwA.cols,CV_8U);

    for (int i=0;i<bwA.rows-1;i++){
        ix.row(i) = abs(bwA.row(i+1)-bwA.row(i));
    }
    ix.row(bwA.rows-1) =abs(bwA.row(bwA.rows-1)-bwA.row(bwA.rows-2));

   for (int i=0;i<bwA.cols-1;i++){
        iy.col(i) = abs(bwA.col(i+1)-bwA.col(i));
    }
    iy.col(bwA.cols-1) =abs(bwA.col(bwA.cols-1)-bwA.col(bwA.rows-2));

    
    Mat disp(bwA.rows/YBOX,bwA.cols/XBOX,CV_8U);
    double tx, ty;
    Mat result(bwA.rows,bwA.cols,CV_8U,Scalar::all(255));
    Mat relabelled_disp(disp.rows,disp.cols,CV_8U,Scalar::all(0));
    
    for(int i=0; i < ix.rows; i+=YBOX){
        for(int j=0; j < ix.cols; j+=XBOX){
            long sum_ix=0,sum_iy=0,sum_ix_iy=0,sum_it_ix=0,sum_it_iy=0;
            for(int x=0; x < YBOX; x++){
                for(int y=0; y < XBOX;y++){
//                    cout <<x<<","<<y<<","<<i<<","<<j<<","<<endl;
                     sum_ix    = sum_ix    + (int) ix.at<uchar>(i+x,j+y)*(int) ix.at<uchar>(i+x,j+y);  
                     sum_iy    = sum_iy    + (int) iy.at<uchar>(i+x,j+y)*(int) iy.at<uchar>(i+x,j+y);  
                     sum_ix_iy = sum_ix_iy + (int) iy.at<uchar>(i+x,j+y)*(int) ix.at<uchar>(i+x,j+y);  
                     sum_it_ix = sum_it_ix + (int) ix.at<uchar>(i+x,j+y)*(int) it.at<uchar>(i+x,j+y);  
                     sum_it_iy = sum_it_iy + (int) iy.at<uchar>(i+x,j+y)*(int) it.at<uchar>(i+x,j+y);  
                }
            }
            tx = (double)(-sum_iy*sum_it_ix + sum_ix_iy*sum_it_iy)/(1+(double)(abs(sum_ix*sum_iy-sum_ix_iy*sum_ix_iy)));
            ty = (double)(-sum_ix*sum_it_iy + sum_ix_iy*sum_it_ix)/(1+(double)(abs(sum_ix*sum_iy-sum_ix_iy*sum_ix_iy)));
            disp.at<uchar>(i/YBOX,j/XBOX) = (tx*tx + ty*ty)>DISP_THRESH?1:0;
        }
    }    
        
//relabelling by voting from 8-neighbour
    for(int i=0;i<disp.rows;i++){
        for(int j=0;j<disp.cols;j++){
            int count=0;
            for(int x=max(0,i-1);x<min(i+1,disp.rows);x++){
                for (int y=max(0,j-1);y<min(j+1,disp.cols);y++){
                    count = count+disp.at<uchar>(x,y);    
                }
            }
            relabelled_disp.at<uchar>(i,j) = count>=CONSENSUS?255:relabelled_disp.at<uchar>(i,j);
        }
    }

    for(int i=0; i<ix.rows;i+=YBOX){
        for(int j=0; j<ix.cols;j+=XBOX){
            for(int x=0;x<YBOX;x++){
                for(int y=0;y<XBOX;y++){
                    result.at<uchar>(i+x,j+y) = relabelled_disp.at<uchar>(i/YBOX,j/XBOX);
                }
            }
        }
    }
    
    return (result); 
}

void save_vid (vector<Mat> a, const string name){
    VideoWriter writer;
    writer.open(name, SAVEFORMAT, 24, a.at(0).size());
    for (int i=0;i<a.size();i++){
        writer.write(a.at(i));
    }
}
