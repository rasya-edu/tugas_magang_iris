#include <iostream>
#include <vector>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/videoio.hpp>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap("/home/rasya/Documents/ROBOTIK/OPENCV/tugas_kordinat/build/video_asli.avi");
    if (!cap.isOpened()) {
        cerr << "No Camera";
        return -1;
    }

    int h_min = 0, s_min = 150, v_min = 150;
    int h_max = 15, s_max = 210, v_max = 255;

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

    Mat frame, hsv_frame, mask;
    Point2f initial_pos(640, 360);
    Point2f robot_position(0, 0);

    Point2f prev_robot_position(0, 0);
    while (cap.read(frame)) {
        if (frame.empty()) {
            break;
        }

        cvtColor(frame, hsv_frame, COLOR_BGR2HSV);
        Scalar min_hsv(h_min, s_min, v_min);
        Scalar max_hsv(h_max, s_max, v_max);

        inRange(hsv_frame, min_hsv, max_hsv, mask);
        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        if (!contours.empty()) {
            vector<Point> largest_contour = contours[0];
            for (size_t i = 1; i < contours.size(); i++) {
                if (contourArea(contours[i]) > contourArea(largest_contour)) {
                    largest_contour = contours[i];
                }
            }

            Rect bounding_rect = boundingRect(largest_contour);

            rectangle(frame, bounding_rect, Scalar(0, 0, 255), 2);
            Point2f square_center = (bounding_rect.tl() + bounding_rect.br()) / 2;

            int offset = 40;
            circle(frame, bounding_rect.tl() + Point(offset, offset), 3, Scalar(255, 0, 0), -1);
            circle(frame, Point(bounding_rect.br().x, bounding_rect.tl().y) + Point(-offset, offset), 3, Scalar(255, 0, 0), -1);
            circle(frame, bounding_rect.br() + Point(-offset, -offset), 3, Scalar(255, 0, 0), -1);
            circle(frame, Point(bounding_rect.tl().x, bounding_rect.br().y) + Point(offset, -offset), 3, Scalar(255, 0, 0), -1);

            Point2f rel_position = square_center - initial_pos;
            Point2f real_world_position(rel_position.x * 10, rel_position.y * 10); // 1px = 10cm
            if (prev_robot_position.x == 0 & prev_robot_position.y == 0) {
                prev_robot_position = real_world_position;
            }

            // Update robot position based on the ball's position
            // robot_position.x = real_world_position.x; // Update X position
            // robot_position.y = real_world_position.y; // Update Y position

            Point2f delta_pos = Point2f(- (real_world_position.x - prev_robot_position.x), real_world_position.y - prev_robot_position.y);
            robot_position += delta_pos;

            string position_label_r = "Posisi Robot: ( X: " + to_string(static_cast<int>(robot_position.x)) + " cm, Y: " + to_string(static_cast<int>(robot_position.y)) + " cm)";
            putText(frame, position_label_r, Point(50, 50), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);

            string position_label_b = "Posisi Bola: (" + to_string(static_cast<int>(real_world_position.x)) + " cm , " + to_string(static_cast<int>(real_world_position.y)) + " cm)";
            putText(frame, position_label_b, Point(square_center.x + 50, square_center.y + 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);

            prev_robot_position = real_world_position;

            imshow("Frame", frame);
            imshow("Mask", mask);
        }

        if (waitKey(1) == 'q') {
            break;
        }
    }

    return 0;
}
