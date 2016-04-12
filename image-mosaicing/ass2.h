#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std; 
/* Usage function; read readme for elaborate instructions */
void usage(char* argv[] );

/* onMouse handler to handle the clicks and get the input */
void onMouse(int, int, int, int, void* ptr);

/* use homography mat 3x3 to map point to a new point*/
Point2f get_homograph(Mat, Point2f);

/* save image with name $num$_temp.jpg name*/
void save_img(int,Mat);
