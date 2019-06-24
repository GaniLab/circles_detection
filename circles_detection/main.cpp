/* This program perform circles detection using opencv Hough transform method
 * the program start by finding edges in image by using gaussian blur and canny edge detector
 * decimal point accuracy of detected circles center however are just one decimal point
*/

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iomanip>
#include <tuple>

#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

// function to detect edge
Mat edge_detection(Mat& image_input, Mat& image_edges)
{
	// convert to gaussian blur
	Mat image_gaussian;

	GaussianBlur(image_input, image_gaussian, Size(5, 5), 1.0);

	// convert to edged image with canny edge detector
	Canny(image_gaussian, image_edges, 50, 80, 3);

	return image_edges;

}

// function to print detected circles center and its radius
void print_circles(ofstream& output, vector<pair<double, double>>& image_points, double& radius)
{
	
	for (int i = 0; i < image_points.size(); i++)
	{
		cout << setprecision(7) << image_points[i].first << " " << image_points[i].second << " " << radius << endl;

		output << setprecision(7) << image_points[i].first << " " << image_points[i].second << endl;
	}
}

int main(int argc, char** argv)
{
	// loading
	const char* filename = argc >= 2 ? argv[1] : "../input/screen_sample.png";

	// Loads an image
	Mat image_input = imread(filename, IMREAD_COLOR);

	// Check if image is loaded successfully
	if (image_input.empty()) {
		cout << " Error opening image" << endl;
		printf(" Program Arguments: [image_name -- default %s] \n", filename);
		return -1;
	}

	/*--------Blurring and canny edge detector---------*/
	Mat image_edges;

	edge_detection(image_input, image_edges);

	namedWindow("image_edge", WINDOW_NORMAL);

	imshow("image_edge", image_edges);

	imwrite("../output/image_edge.jpg", image_edges);

	/*--------detect circles, draw and get the centers---------*/

	// convert_to_gray
	Mat gray;

	cvtColor(image_input, gray, COLOR_BGR2GRAY);

	// reduce_noise
	medianBlur(gray, gray, 5);

	// houghcircles
	vector<Vec3f> circles;

	// minimum and maximum circle radius to be detected in pixels
	// for the sample in this program its ---> r_min=20 and r_max=25
	double r_min, r_max;

	cout << "define minimum circle radius (in pixel) to be detected: ";
	cin >> r_min;

	cout << "\ndefine minimum circle radius (in pixel) to be detected: ";
	cin >> r_max;

	cout << "\n\ncircle center points and radius:\n";

	HoughCircles(gray,
		circles,
		HOUGH_GRADIENT,
		0.0001,
		gray.rows / 10000.00,  // change this value to detect circles with different distances to each other
		100.00, 40.00, r_min, r_max // the last two parameters (min_radius & max_radius) can be changed for adjustment
	);

	// outputting detected circles
	ofstream output("../output/detected_circle.txt");

	// drawing circles
	for (double i = 0; i < circles.size(); i++)
	{
		Vec3d c = circles[i];
		Point2d center = Point2d(c[0], c[1]);

		// circle center
		circle(image_input, center, 0.01, Scalar(0, 100, 255), 3);

		// circle outlining
		double radius = c[2];
		circle(image_input, center, radius, Scalar(255, 100, 255), 3, LINE_AA);

		// create image points of 2D vectors
		vector<pair<double, double>> image_points;

		double u = center.x;
		double v = center.y;

		image_points.push_back(make_pair(u, v));

		print_circles(output, image_points, radius);

	}

	output.close();

	// display
	namedWindow(filename, WINDOW_NORMAL);

	imshow(filename, image_input);

	imwrite("../output/detected_circles.jpg", image_input);

	waitKey(0);

	return 0;
}