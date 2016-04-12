#include "ass1.h"
#include "params.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){

	const char* file_name1, *file_name2;
	if(argc != 3){
		usage( argv);
		exit(1);
	}

// read images
	file_name1 = argv[1];
	Mat a = imread(file_name1, CV_LOAD_IMAGE_COLOR);

	file_name2 = argv[2];
	Mat b = imread(file_name2, CV_LOAD_IMAGE_COLOR);

// clone them for the displays, as we will be dotting the selected pixels for triangulation. 
	Mat a_clone = a.clone();
	Mat b_clone = b.clone();

	namedWindow("image 1", WINDOW_NORMAL );
	cout << a.size()<<std::endl;
	imshow("image 1",a);

	namedWindow("image 2", WINDOW_NORMAL );
	cout << b.size()<<std::endl;
	imshow("image 2",b);

// get mouse input and store in point_1 and point_2 vectors
// Pressing any key will proceed you to selecting point on next image
// If you press esc after selecting the point on second image, it will exit and 
// start the transformation
  int key;
  vector <Point> point_1, point_2;
  Point p;

  do{
    setMouseCallback("image 1", onMouse, &p);
    waitKey(0);
    cout << "in image 1 at " <<p << std::endl;
    point_1.push_back(p);
    circle(a_clone, p, 2, DOTCOLOR,-1,0);
    imshow ("image 1", a_clone);

    setMouseCallback("image 2",onMouse,&p);
    waitKey(0);
    cout <<"in image 2 at "<< p <<std::endl;
    point_2.push_back(p);
    circle(b_clone, p, 2, DOTCOLOR,-1,0);
    imshow ("image 2", b_clone);

    key = waitKey(0);
  }while(key!=27);

// push the four corner points, they are needed anyways
  Size sz= a.size();
  point_1.push_back(Point(0,0));
  point_1.push_back(Point(sz.width-1,0));
  point_1.push_back(Point(0,sz.height-1));
  point_1.push_back(Point(sz.width-1,sz.height-1));

// should the size be same?, NO
  sz= b.size();
  point_2.push_back(Point(0,0));
  point_2.push_back(Point(sz.width-1,0));
  point_2.push_back(Point(0,sz.height-1));
  point_2.push_back(Point(sz.width-1,sz.height-1));

// delunay triangulation; This is how it is done
  Rect rect(0,0,a.size().width, a.size().height);
  Subdiv2D subdiv(rect);
  int i;
  for (i=0;i<point_1.size();i++)
    subdiv.insert(point_1[i]);

// get the delunay triangles; and draw on the image 1
  vector <Vec6f> triangleList;
  get_triangles_from_subdiv(subdiv, triangleList, a.size());
  draw_subdiv(a_clone,triangleList, LINECOLOR);
  imshow ("image 1", a_clone);

// generate generic triangles, it has indices rather than the co-ordinates
// so that we can generate corresponding triangles on other image
  vector <Vec3i> genericTriangleList;
  vector <Point> pt(3);
  for (i=0;i<triangleList.size();i++){
    Vec6f t = triangleList[i];
    pt[0] = Point(cvRound(t[0]),cvRound(t[1]));
    pt[1] = Point(cvRound(t[2]),cvRound(t[3]));
    pt[2] = Point(cvRound(t[4]),cvRound(t[5]));
    Vec3i temp;
    temp[0] = search_pt(point_1, pt[0]);
    temp[1] = search_pt(point_1, pt[1]);
    temp[2] = search_pt(point_1, pt[2]);
    genericTriangleList.push_back( temp);
  }


// draw triangle on final image  
  vector <Vec6f> newTriangleList;
  find_triangles(genericTriangleList,point_2,newTriangleList);
  draw_subdiv(b_clone, newTriangleList, LINECOLOR);
  imshow("image 2", b_clone);
	cout <<endl;
// done with all the drawings etc. We now start the real work.
// First get the triangle on initial and final images using genericTriangles.
// This is a redundant step as we already have them in some other variables
	vector <Vec6f> triangles1;
  vector <Vec6f> triangles2;
  find_triangles(genericTriangleList, point_1, triangles1);
  find_triangles(genericTriangleList, point_2, triangles2);

// declare the storage elements to store the pixel correspondences
  Mat blend_1(sz.height, sz.width, CV_8UC3);
  Mat blend_2(sz.height, sz.width, CV_8UC3);
  Mat image  (sz.height, sz.width, CV_8UC3);
	string name;
	namedWindow("Morph", WINDOW_NORMAL );
	namedWindow("blend1", WINDOW_NORMAL );
	namedWindow("blend2", WINDOW_NORMAL );
	for(i=1;i<LENGTH;i++){
		vector <Vec6f> triangles;
// Get the interpolated triangle			
		interpolate_triangle(triangles1, triangles2,(float)i/LENGTH,triangles);
// Get the pixel correspondence to the interpolated triangle
    get_color(a,triangles1, triangles, blend_1);
    get_color(b,triangles2, triangles, blend_2);
// Generate image from the two pixel correspondences and show the blends
		imshow("blend2", blend_2);
		imshow("blend1", blend_1);
    generate_image(blend_1,blend_2,(float)i/LENGTH,image);
		imshow("Morph", image);
		waitKey(300);
// Save image
		ostringstream convert;
    convert << i;
    name = convert.str()+"_save.jpg";
    imwrite(name,image);
//	cout <<i<<endl;
	 }
	imwrite("0_save.jpg",a);
	ostringstream convert;
	convert <<LENGTH;
	name = convert.str()+"_save.jpg";
	imwrite(name,b);
	cout <<"Morph Generated"<<std::endl;
// save the initial and final images by the name save_0 and save_LENGTH
	
// display the morph
	key = waitKey(0);
	while(key!=27){
		for (i=0;i<=LENGTH;i++){
			ostringstream convert;
			convert << i;
			name = convert.str()+"_save.jpg";
			Mat im = imread(name, CV_LOAD_IMAGE_COLOR); 
			imshow("Morph",im);
			waitKey(150);
		}
		key =waitKey(0);
	}

	return 1 ;
}
