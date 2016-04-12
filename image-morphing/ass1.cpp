#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv; 

void usage(char* argv[]){
  cout << "usage "<<argv[0] <<" <image 1> <image 2>"<<std::endl;  
}

void onMouse(int e, int x, int y, int d, void *ptr ){
  Point *p  = (Point* )ptr;
  p->x      = x;
  p->y      = y;
}

void get_triangles_from_subdiv (Subdiv2D& subdiv, vector <Vec6f> &triangleList, Size sz){
  subdiv.getTriangleList(triangleList);
  int i=0,k;
  while(i<triangleList.size()){
    int flag=0;
    Vec6f t = triangleList[i];

    for(k=0;k<6;k+=2){
      if (t[k] >=sz.width|| t[k]<0){
        flag =1;
      }
    }

    for(k=1;k<6;k+=2){
      if (t[k] >=sz.height|| t[k]<0){
        flag =1;
      }
    }

    if (flag==1){
      triangleList.erase(triangleList.begin()+i);
    }else{
      i++;
    }

  }
}

void draw_subdiv( Mat& img, vector<Vec6f> triangleList, Scalar delaunay_color ){
  vector<Point> pt(3);
  for( size_t i = 0; i < triangleList.size(); i++ ){
    Vec6f t = triangleList[i];
    pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
    pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
    pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
    line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
    line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
    line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
  }
}

int search_pt(vector <Point> &vec, Point &p){
  int idx = 0 ;
  float min_dis = cv::norm(vec[0]- p );

  int i;
  for (i=1; i<vec.size();i++){
    float dis = norm (vec[i]-p);

    if (min_dis > dis ){
      min_dis = dis;
      idx = i;
    }
  }
  return idx;
}

void find_triangles(vector<Vec3i> & generic, vector<Point> &p , vector<Vec6f>&triangle){
  for (int i=0;i<generic.size();i++){
    Vec6f t;
    Vec3i T = generic[i];

    t[0] = p[T[0]].x; t[1] = p[T[0]].y;
    t[2] = p[T[1]].x; t[3] = p[T[1]].y;
    t[4] = p[T[2]].x; t[5] = p[T[2]].y;

    triangle.push_back(t);
  }
}

void get_affine_transforms(vector<Vec6f>& T1, vector <Vec6f>& T2,vector<Mat>& t){
  int i;
  for(i=0;i<T1.size();i++){
    vector <Point2f> pt1(3);
    vector <Point2f> pt2(3);
    Vec6f temp1, temp2;

    temp1 = T1[i]; temp2 = T2[i];

    for (int k=0; k<3;k++){
      pt1[k] = Point(temp1[2*k],temp1[2*k+1]);
      pt2[k] = Point(temp2[2*k],temp2[2*k+1]);
    }

    t.push_back(getAffineTransform(pt1,pt2));
  }
}

bool same_side(Point &p1,Point &p2,Point &a, Point &b){
    Point A = b-a;
    Point B = p1-a;
    Point C = p2-a;

    float cp1 = A.x*B.y - A.y*B.x;
    float cp2 = A.x*C.y - A.y*C.x;

    if ((cp1*cp2) >= 0)
      return true;
    else
      return false;
}

bool point_in_triangle(Point &p, Vec6f &t){
  Point a = Point(t[0],t[1]);
  Point b = Point(t[2],t[3]);
  Point c = Point(t[4],t[5]);

  if (same_side(p,a, b,c) && same_side(p,b, a,c) && same_side(p,c, a,b))
    return true;
  else
    return false;
}

int triangle_point(Point &p, vector<Vec6f>& T ){
  int i=0;
  for(i=0;i<T.size();i++){
    if (point_in_triangle(p,T[i]))
      return i;
  }
//should not come here
  cout <<"Problem in triangle_point, returning " <<i-1<<std::endl;
  return i-1;
}

void interpolate_triangle(vector<Vec6f> &T1, vector<Vec6f> &T2, float factor, vector<Vec6f> &T){
  for(int i=0; i<T1.size();i++){
    T.push_back(T1[i]*(1-factor)+T2[i]*factor);
  }
}

void get_color(Mat &img,vector<Vec6f> &triangle1, vector<Vec6f> &triangle2,Mat &blend){
  Size sz = img.size();
  vector<Mat> trans;
  get_affine_transforms(triangle2, triangle1, trans);
  blend.create(sz.height,sz.width,CV_8UC3);
  int i,j;
  for(i=0;i< sz.width;i++){
    for(j=0;j<sz.height;j++){
      Point p = Point(i,j);
      int idx = triangle_point(p,triangle2);
      Mat transform = trans[idx];
      int x = cvRound(transform.at<double>(0,0)*i+transform.at<double>(0,1)*j+transform.at<double>(0,2));
      int y = cvRound(transform.at<double>(1,0)*i+transform.at<double>(1,1)*j+transform.at<double>(1,2));
      if(x<0){x=0;}
      if(x>=sz.width){x=sz.width-1;}
      if(y<0){y=0;}
      if(y>=sz.height){y=sz.height-1;}
      blend.at<Vec3b>(j,i) = img.at<Vec3b>(y,x);
    }
  }
}

void generate_image(Mat &blend1, Mat &blend2,float factor,Mat &img){
  Size sz = blend1.size();
  img.create(sz.height, sz.width, CV_8UC3);
  int channels = blend1.channels();
  int nRows = blend1.rows;
  int nCols = blend1.cols * channels;
  int i,j;
  uchar *p1,*p2,*p;
  for(i=0;i<nRows;++i){
    p1 = blend1.ptr<uchar>(i);
    p2 = blend2.ptr<uchar>(i);
    p  = img.ptr<uchar>(i);
    for(j=0;j<nCols;++j){
      p[j] = (uchar)((float)p1[j]*(1-factor) + (float)p2[j]*factor);
    }
  }
}

