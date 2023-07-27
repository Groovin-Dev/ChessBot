//
// Created by meliodas on 7/27/23.
//

#include "x11wrapper.h"


cv::Mat GrabScreen() {
    Display *display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);

    XWindowAttributes gwa;
    XGetWindowAttributes(display, root, &gwa);
    int width = gwa.width;
    int height = gwa.height;

    XImage *img = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);

    cv::Mat mat = cv::Mat(height, width, CV_8UC4, img->data);

    // Convert BGRA to BGR
    cv::Mat matBGR;
    cv::cvtColor(mat, matBGR, cv::COLOR_BGRA2BGR);

    // Cleanup
    XDestroyImage(img);
    XCloseDisplay(display);

    return matBGR;
}