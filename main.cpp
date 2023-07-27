#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#undef Bool
#include <QtSvg/QSvgRenderer>
#include <QtGui/QImage>
#include <QtGui/QPainter>

#include "x11wrapper.h"

cv::Mat loadImageFromSVG(const std::string &filename) {
    QSvgRenderer renderer(QString::fromStdString(filename));

    QImage image(renderer.defaultSize(), QImage::Format_ARGB32);
    QPainter painter(&image);
    renderer.render(&painter);

    cv::Mat mat(image.height(), image.width(), CV_8UC4, (void *) image.constBits(), image.bytesPerLine());
    cv::cvtColor(mat, mat, cv::COLOR_BGRA2BGR);

    return mat;
}

void detectPieces(cv::Mat& board) {
    std::vector<std::string> pieceFilenames = {"bB.svg", "bK.svg", "bN.svg", "bP.svg", "bQ.svg", "bR.svg",
                                               "wB.svg", "wK.svg", "wN.svg", "wP.svg", "wQ.svg", "wR.svg"};

    std::vector<cv::Mat> pieceImages;
    for (const auto &filename: pieceFilenames) {
        std::string filepath = "pieces/" + filename;
        cv::Mat pieceImage = loadImageFromSVG(filepath);
        pieceImages.push_back(pieceImage);
    }

    for (const auto &pieceImage: pieceImages) {
        cv::Mat result;
        cv::matchTemplate(board, pieceImage, result, cv::TM_CCOEFF_NORMED);

        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        if (maxVal > 0.95) { // This is a threshold, adjust as needed
            // Detected a piece
            // Draw a rectangle around the detected piece
            cv::rectangle(board, cv::Rect(maxLoc, maxLoc + cv::Point(pieceImage.cols, pieceImage.rows)), cv::Scalar(0, 255, 0), 2);
        }
    }
}

cv::Mat createColorMask(const cv::Mat &img, const cv::Vec3b &color, int threshold) {
    cv::Mat mask;
    cv::Vec3b thresholdVec(threshold, threshold, threshold);
    cv::inRange(img, color - thresholdVec, color + thresholdVec, mask);
    return mask;
}

cv::Rect DetectBoard(const cv::Mat &img) {
    // Convert image to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Find contours in the grayscale image
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(gray, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Find the largest contour (assume this is the board)
    double maxArea = 0.0;
    std::vector<cv::Point> maxContour;
    for (const auto &contour: contours) {
        double area = cv::contourArea(contour);
        if (area > maxArea) {
            maxArea = area;
            maxContour = contour;
        }
    }

    // Create a bounding rectangle for the largest contour
    cv::Rect boardRect = cv::boundingRect(maxContour);

    // Return the bounding box
    return boardRect;
}

cv::Mat CropToBoard(const cv::Mat &img, cv::Rect boardBox) {
    // Crop the image to the board using the bounding box
    return img(boardBox);
}

int main() {
    // Grab screen
    cv::Mat img = GrabScreen();

    // Convert the hexadecimal color values to BGR
    cv::Vec3b lightSquareColor(181, 217, 240); // #F0D9B5 in BGR
    cv::Vec3b darkSquareColor(99, 136, 181);  // #B58863 in BGR
    cv::Vec3b currentMoveLightSquareColor(106, 210, 205); // #CDD26A in BGR
    cv::Vec3b currentMoveDarkSquareColor(58, 162, 170);   // #AAA23A in BGR

    // Define threshold for color detection
    int colorThreshold = 1; // Adjust this value based on how exact the match should be

    // Create masks based on the color ranges
    cv::Mat lightSquareMask = createColorMask(img, lightSquareColor, colorThreshold);
    cv::Mat darkSquareMask = createColorMask(img, darkSquareColor, colorThreshold);
    cv::Mat currentMoveLightSquareMask = createColorMask(img, currentMoveLightSquareColor, colorThreshold);
    cv::Mat currentMoveDarkSquareMask = createColorMask(img, currentMoveDarkSquareColor, colorThreshold);

    // Combine the masks
    cv::Mat mask = lightSquareMask | darkSquareMask | currentMoveLightSquareMask | currentMoveDarkSquareMask;

    // Apply the mask
    cv::Mat result;
    img.copyTo(result, mask);

    // Detect the board
    cv::Rect boardBoundingBox = DetectBoard(result);

    // Crop the original image to the board
    cv::Mat board = CropToBoard(img, boardBoundingBox);

    // Call the piece detection function
    detectPieces(board);

    // Show the result
    cv::imshow("Board", board);
    cv::waitKey();

    return 0;
}