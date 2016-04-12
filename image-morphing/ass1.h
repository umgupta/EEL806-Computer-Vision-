#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std; 
/* Usage function; read readme for elaborate instructions */
void usage(char* argv[] );

/* onMouse handler to handle the clicks and get the input */
void onMouse(int, int, int, int, void* ptr);

/* wrapper over subdiv's getTriangleList to filter out infinity points or points that are out of image 
It changes triangleList to contain only relevant point */
void get_triangles_from_subdiv (Subdiv2D&, vector <Vec6f>&, Size);

/* draw delaunay subdivisions/triangles on image */
void draw_subdiv(Mat&, vector<Vec6f>, Scalar);

/* locates the vertex nearest to the point; minimum distance point */
int search_pt(vector <Point> &, Point &);

/* generates coordinate triangles from generic list (indices) of triangles */
void find_triangles(vector<Vec3i> & , vector<Point> & , vector<Vec6f>&);

/* get ordered vector of affine transforms vector<Mat> from triangles in T1 to that in T2*/
void get_affine_transforms(vector<Vec6f>& T1, vector <Vec6f>& T2,vector<Mat>& t);

/* returns true if p1,p2 lie on same side of line joining a and b*/
bool same_side(Point &p1,Point &p2,Point &a, Point &b);

/* returns true if point is inside the triangle */
bool point_in_triangle(Point &p, Vec6f &t);

/* returns the index of triangle that has the point p*/
static int triangle_point(Point &p, vector<Vec6f>& T );

/* interpolates the coordinates of T1 and T2 triangle to generate the intermediate triangles
Linear interpolation i.e. T1*(1-factor)+T2*factor*/
void interpolate_triangle(vector<Vec6f> &T1, vector<Vec6f> &T2, float factor, vector<Vec6f> &T);

/* Return the color (pixel correspondence from img to new image, 
where the img and new image are related by affine transformations of point in triangle1 and triangle2 */
void get_color(Mat &img,vector<Vec6f> &triangle1, vector<Vec6f> &triangle2,Mat &blend);

/* When you have correspondence both from final and initial triangle, you need to generate image
by blending the colors which is done by this function, Linear interpolation color1(1-factor) + color2(factor)
new image is returned in img */
void generate_image(Mat &blend1, Mat &blend2,float factor,Mat &img);
