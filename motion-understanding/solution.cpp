#include "ass4.h"
#include "params.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <math.h>

using namespace cv;
using namespace std;
CvScalar rectColor;
int main(int argc, char* argv[]){

// exit if 2 files are not given 
	const char* file_name;
	if(argc != 2){
		usage( argv);
		exit(1);
	}

    VideoCapture video = VideoCapture(argv[1]);
    Mat frame;

    vector<Mat> curFrame;
    vector<Mat> processedFrame;
    Mat nextFrame;
    Mat nextNextFrame;
    Mat nextNextNextFrame;

    cout <<"Reading Video...."<<endl;
    bool hasNextFrame = video.read(frame);
    while(hasNextFrame){
        curFrame.push_back(frame.clone());
        hasNextFrame = video.read(frame);
    }
    cout <<"Video Read"<<endl;

    cout <<"Starting labelling ..."<<endl;
    for (int i=0;i<curFrame.size()-SUBSAMPLING;i++){
        processedFrame.push_back(process(curFrame.at(i),curFrame.at(i+SUBSAMPLING)));
    }
    cout <<"Preliminary labels done by calculating displacements and consensus voting"<<endl;
    cout <<"Output mask on the screen"<<endl;

    for(int i=0;i<processedFrame.size();i++){   
        imshow("video",curFrame.at(i));
        imshow("Processed video",processedFrame.at(i));
        waitKey(1000/FPS);
    }    
 
    cout <<"Starting temporal labelling"<<endl;
    for(int i=0;i<processedFrame.size();i++){
        Mat curTemp, nextTemp, prevTemp;
        if (i==0){
            curTemp  = processedFrame.at(i);
            nextTemp = processedFrame.at(i+1);       
            prevTemp = processedFrame.at(i+1);
        }else if (i==processedFrame.size()-1){
            curTemp  = processedFrame.at(i);
            nextTemp = processedFrame.at(i-1);
            prevTemp = processedFrame.at(i-1);
        }else{
            curTemp  = processedFrame.at(i);
            nextTemp = processedFrame.at(i+1);
            prevTemp = processedFrame.at(i-1);
        }
        
        for(int i=0;i<curTemp.rows;i+=YBOX){
            for(int j=0;j<curTemp.cols;j+=XBOX){
                if (curTemp.at<uchar>(i,j)>0){
                    int count =0;
                    for(int x=max(0,i-YBOX);x<min(curTemp.rows,i+YBOX);x+=YBOX){
                        for(int y=max(0,j-XBOX);y<min(curTemp.cols,j+XBOX);y+=XBOX){
                            count = nextTemp.at<uchar>(x,y)>0?count+1:count;
                            count = prevTemp.at<uchar>(x,y)>0?count+1:count;
                        }
                    }
                    if (count < 1){
                        for(int x=0;x<YBOX;x++){
                            for(int y=0;y<XBOX;y++){
                                curTemp.at<uchar>(i+x,j+y) =0;
                            }
                        }

                    }
                } 
            }
        }
    }
    cout <<"Temporal labelling done"<<endl;
    cout <<"Output mask on the screen"<<endl;
    for(int i=0;i<processedFrame.size();i++){   
        imshow("video",curFrame.at(i));
        imshow("Processed video",processedFrame.at(i));
        waitKey(1000/FPS);
    }    

    cout <<"Relabelling by consensus voting"<<endl;
    for(int i=0;i<processedFrame.size();i++){
        Mat curTemp = processedFrame.at(i).clone();
        Mat temp    = processedFrame.at(i);
        for(int i=0;i<curTemp.rows;i+=YBOX){
           for(int j=0;j<curTemp.cols;j+=XBOX){
                int count=0;
                for(int x=max(0,i-2);x<min(i+2,curTemp.rows);x++){
                    for (int y=max(0,j-2);y<min(j+2,curTemp.cols);y++){
                        count = count+curTemp.at<uchar>(x,y)/255;    
                    }
                }
                for (int x=i;x<i+YBOX;x++){
                    for (int y=j;y<j+XBOX;y++){
                        temp.at<uchar>(x,y) = count>=RECONSENSUS?255:0;
                    }   
                }    
            }
        }
    }
    cout <<"showing relabelling by consensus results; output mask on the screen"<<endl;
    for(int i=0;i<processedFrame.size();i++){   
        imshow("video",curFrame.at(i));
        imshow("Processed video",processedFrame.at(i));
        waitKey(1000/FPS);
    }    

 

    cout <<"final_results"<<endl;
    vector<Mat> finalFrames;
    for(int i=0;i<processedFrame.size();i++){
        Mat curTemp = curFrame.at(i).clone();
        Mat proTemp = processedFrame.at(i);
        for(int x=0;x<curTemp.rows;x++){
            for(int y=0;y<curTemp.cols;y++){
                if (proTemp.at<uchar>(x,y)>0){
                    Vec3b & color = curTemp.at<Vec3b>(x,y);
                    color[0] = MOTIONCOLOR_B;
                    color[1] = MOTIONCOLOR_G;
                    color[2] = MOTIONCOLOR_R;
                    
                }
            }
        }
        finalFrames.push_back(curTemp);
    }
    
    string filename = "color_masked_motion_umcp.mpg";
    cout <<"On the screen and saved as "<<filename<<endl; //filename<<endl;
    for(int i=0;i<finalFrames.size();i++){   
        imshow("video",curFrame.at(i));
        imshow("Processed video",finalFrames.at(i));
        waitKey(1000/FPS);
    }    
    
    save_vid(finalFrames,filename);


    cout<<"Removing the moving pixels"<<endl;
    vector<Mat> unmotion_frames;
        
    for(int i=0;i<processedFrame.size();i++){
        unmotion_frames.push_back(curFrame.at(i).clone());
    
        Mat temp = processedFrame.at(i);
        for(int i=0;i<temp.rows;i+=YBOX){
            for(int j=0;j<temp.cols;j+=XBOX){
                if (temp.at<uchar>(i,j)>0){
                    int non_motion_count =0;
                    int a = i+1;
                    while(non_motion_count<NONMOTION_NUM && a<processedFrame.size() ){
                        if (processedFrame.at(a).at<uchar>(i,j)==0){non_motion_count++;}
                        a=a+1;
                    }
                    if (non_motion_count <NONMOTION_NUM){
                        a=i-1;
                        non_motion_count =0;
                        while(non_motion_count<NONMOTION_NUM && a>-1 ){
                            if (processedFrame.at(a).at<uchar>(i,j)==0){non_motion_count++;}
                        a=a-1;
                        }
                    }
                    if (non_motion_count>=NONMOTION_NUM && a>-1 && a<processedFrame.size()){
                        for(int x=i;x<i+YBOX;x++){
                            for(int y=j;y<j+XBOX;y++){
                                unmotion_frames.back().at<Vec3b>(x,y) = curFrame.at(a).at<Vec3b>(x,y);                
                            }
                        }   
                    }
                }
            }   
        }
    }
    filename = "unmotion_umcp.mpg";
    cout <<"Unmotioned the video; output on screen and saved as "<<filename<<endl; 
     for(int i=0;i<unmotion_frames.size();i++){   
        imshow("video",curFrame.at(i));
        imshow("Processed video",unmotion_frames.at(i));
        waitKey(1000/FPS);
    }    
    save_vid(unmotion_frames,filename);

    vector<Mat> carpet_frames;
        
    for(int i=0;i<processedFrame.size();i++){
        carpet_frames.push_back(curFrame.at(i).clone());
    
        Mat temp = processedFrame.at(i);
        for(int i=0;i<temp.rows;i++){
            for(int j=0;j<temp.cols;j++){
                if( (0.67*j+i-289 <=0) && (1.65*j-i-53<=0) && (0.53*j+i-213>=0)){
                    if (temp.at<uchar>(i,j)==0){
                        Vec3b & color = carpet_frames.back().at<Vec3b>(i,j);
                        color[0] = CARPET_COLOR_B;
                        color[1] = CARPET_COLOR_G;
                        color[2] = CARPET_COLOR_R;                            
                    }
                }
            }   
        }
    }
    filename = "carpet_umcp.mpg";
    cout <<"Put a carpet at the parking line; output on screen and saved as "<<filename<<endl; 
     for(int i=0;i<carpet_frames.size();i++){   
        imshow("video",curFrame.at(i));
        imshow("Processed video",carpet_frames.at(i));
        waitKey(1000/FPS);
    }    
    save_vid(carpet_frames,filename);



    return 0;
}
