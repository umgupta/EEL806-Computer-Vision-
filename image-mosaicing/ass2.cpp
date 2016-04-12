#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv; 

void usage(char* argv[]){
  cout << "usage "<<argv[0] <<" <image 1> <image 2> ....."<<std::endl;  
}

void onMouse(int e, int x, int y, int d, void *ptr ){
  Point *p  = (Point* )ptr;
  p->x      = x;
  p->y      = y;
}

Point2f get_homograph(Mat H, Point2f p){
	Point2f point;
	point.x = H.at<double>(0,0)*p.x+H.at<double>(0,1)*p.y+H.at<double>(0,2);
	point.y = H.at<double>(1,0)*p.x+H.at<double>(1,1)*p.y+H.at<double>(1,2);
	float fact = H.at<double>(2,0)*p.x+H.at<double>(2,1)*p.y+H.at<double>(2,2);
	point.x = point.x/fact; point.y = point.y/fact;
	return point;
}

void save_img (int num,Mat a){
	string name;
	ostringstream convert;
  convert << num;
  name = convert.str()+"_temp.jpg";
  imwrite(name,a);
}
