#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std; 
/* Usage function; read readme for elaborate instructions */
void usage(char* argv[] );

/* onMouse handler to handle the clicks and get the input */
void onMouse(int, int, int, int, void* ptr);

/*Get top and bottom points*/
Mat process(Mat a, Mat b);

/* save image with name $num$_temp.jpg name*/
void save_vid(vector<Mat>,const string name);


