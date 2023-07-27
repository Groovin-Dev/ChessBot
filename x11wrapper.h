#ifndef CHESSBOT_X11WRAPPER_H
#define CHESSBOT_X11WRAPPER_H

#include "x11wrapper.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/mat.hpp>

cv::Mat GrabScreen();

#endif //CHESSBOT_X11WRAPPER_H
