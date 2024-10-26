#include <iostream>
#include <vector>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/videoio.hpp>

using namespace cv;
using namespace std;

int main() {

    VideoCapture cap("/dev/v4l/by-id/usb-Azurewave_USB2.0_VGA_UVC_WebCam-video-index0");
    if (!cap.isOpened()) {
        cerr << "No Camera";
        return -1;
    }

    // int h_min = 136, s_min = 84, v_min = 35;
    // int h_max = 255, s_max = 235, v_max = 255;
    int h_min = 0, s_min = 55, v_min = 150;
    int h_max = 10, s_max = 210, v_max = 255;

    // namedWindow("Trackbar");
    // createTrackbar("Hmin", "Trackbar", &h_min, 255);
    // setTrackbarPos("Hmin", "Trackbar", 75);
    // createTrackbar("Smin", "Trackbar", &s_min, 255);
    // setTrackbarPos("Smin", "Trackbar", 75);
    // createTrackbar("Vmin", "Trackbar", &v_min, 255);
    // setTrackbarPos("Vmin", "Trackbar", 75);
    // createTrackbar("Hmax", "Trackbar", &h_max, 255);
    // setTrackbarPos("Hmax", "Trackbar", 75);
    // createTrackbar("Smax", "Trackbar", &s_max, 255);
    // setTrackbarPos("Smax", "Trackbar", 75);
    // createTrackbar("Vmax", "Trackbar", &v_max, 255);
    // setTrackbarPos("Vmax", "Trackbar", 75);

    Mat frame, f_hsv, f_mask, f_res;
    while (cap.read(frame)) {

        cap >> frame;
        if (frame.empty()) {
            break;
        }

        cvtColor(frame, f_hsv, COLOR_BGR2HSV);

        Scalar min(h_min, s_min, v_min);
        Scalar max(h_max, s_max, v_max);

        inRange(f_hsv, min, max, f_mask);
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(f_mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        if (!contours.empty()) {
            
            vector<Point> largest_contour = contours[0];
            for (size_t i = 1; i < contours.size(); i++) {
                if (contourArea(contours[i]) > contourArea(largest_contour)) {
                    largest_contour = contours[i];
                }
            }

            Point2f center;
            float radius;
            minEnclosingCircle(largest_contour, center, radius);

            circle(frame, center, (int)radius, cv::Scalar(255, 0, 0), 6);

            float diameter = radius*2;
            float focalLen = (120*23)/3.5;
            float dist = (3.5*focalLen)/diameter;

            string sizeLabel = "Size: " + to_string((int)(diameter)) + " px";
            string distLabel = "Approx Distance: " + to_string((int)(dist)) + " cm";
            putText(frame, sizeLabel, Point(center.x+50, center.y+radius+20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2.2);
            putText(frame, distLabel, Point(center.x+50, center.y+radius+40), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 0, 0), 2.2);
        
        }

        imshow("Frame", frame);
        imshow("Mask", f_mask);

        if (waitKey(1) == 'q') {
            break;
        }
    }

    return 0;
}