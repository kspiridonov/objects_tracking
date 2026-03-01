#include "optical_flow_frame_processor.h"
#include <opencv2/highgui.hpp>
#include "logger/logger.h"

void OpticalFlowFrameProcessor::processFrame(Mat &currentFrame) {
    if (previousFrame.empty()) {
        previousFrame = currentFrame.clone();
        return;
    }

    Mat currentGray;
    Mat previousGray;
    cvtColor(currentFrame, currentGray, COLOR_BGR2GRAY);
    cvtColor(previousFrame, previousGray, COLOR_BGR2GRAY);
    if (!previousGray.empty()) {
        // Initialize parameters for the optical flow algorithm
        float pyrScale = 0.5;
        int numLevels = 3;
        int windowSize = 15;
        int numIterations = 3;
        int neighborhoodSize = 5;
        float stdDeviation = 1.2;
        Mat flowImage(previousGray.size(), CV_32FC2);
        // Calculate optical flow map using Farneback algorithm
        calcOpticalFlowFarneback(previousGray, currentGray, flowImage, pyrScale,
                                 numLevels, windowSize, numIterations,
                                 neighborhoodSize, stdDeviation, 0);
        Mat result = previousFrame.clone();
        Mat clonedFlowImage = flowImage.clone();
        vector<Rect> rects = findRects(clonedFlowImage);
        auto rectsInfo = getOpticalFlowRectsInfo(result, flowImage, rects);

        previousFrame = currentFrame.clone();
        if (showDebugInfo) {
            vector<Rect> rectsToDraw;
            vector<Rect> movedRectsToDraw;
            for (auto &rect: rectsInfo) {
                rectsToDraw.push_back(rect.getCurrentRect());
                movedRectsToDraw.push_back(rect.getMovedToPreviousPositionRect());
            }
            drawRectanglesOnResult(rectsToDraw, result, Scalar(0, 255, 0));
            drawRectanglesOnResult(movedRectsToDraw, result, Scalar(255, 0, 0));
        }
        fireFrameProcessed(result, rectsInfo);
    }
}

void OpticalFlowFrameProcessor::drawOpticalFlowOnResult(const Mat &flowImage, Mat &result, int *vectorX, int *vectorY) {
    int stepSize = 16;
    Scalar color = Scalar(0, 255, 0);

    // Draw the uniform grid of points on the input image along with the motion
    // vectors
    for (int y = 0; y < result.rows; y += stepSize) {
        for (int x = 0; x < result.cols; x += stepSize) {
            // Circles to indicate the uniform grid of points
            int radius = 2;
            int thickness = -1;
            circle(result, Point(x, y), radius, color, thickness);

            // Lines to indicate the motion vectors
            Point2f pt = flowImage.at<Point2f>(y, x);
            line(result, Point(x, y), Point(cvRound(x + pt.x), cvRound(y + pt.y)),
                 color);
        }
    }
    if (vectorX != nullptr && vectorY != nullptr) {
        int centerX = result.cols / 2;
        int centerY = result.rows / 2;
        // Circles to indicate the uniform grid of points
        int radius = 2;
        int thickness = -1;
        circle(result, Point(centerX, centerY), radius, Scalar(255, 0, 0), thickness);
        line(result, Point(centerX, centerY), Point(centerX + *vectorX, centerY + *vectorY), Scalar(255, 0, 0));
    }
}

vector<Rect> OpticalFlowFrameProcessor::findRects(const Mat &flowImage) {
    vector<Rect> rects;
    double threshold = 4.0;
    cv::Mat magnitude, angle;
    std::vector<cv::Mat> flow_channels;
    cv::split(flowImage, flow_channels);
    cv::cartToPolar(flow_channels[0], flow_channels[1], magnitude, angle);
    cv::Mat motion_mask = magnitude > threshold;
    cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(40, 40));
    cv::morphologyEx(motion_mask, motion_mask, cv::MORPH_CLOSE, structuringElement);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(motion_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Рисуем ограничивающие прямоугольники
    for (size_t i = 0; i < contours.size(); i++) {
        if (cv::contourArea(contours[i]) > 1000) {
            // Игнорируем маленькие контуры
            cv::Rect bounding_box = cv::boundingRect(contours[i]);
            rects.push_back(bounding_box);
        }
    }
    return rects;
}

void OpticalFlowFrameProcessor::drawRectanglesOnResult(vector<Rect> &rects, Mat &result, Scalar color) {
    // Рисуем ограничивающие прямоугольники
    for (size_t i = 0; i < rects.size(); i++) {
        if (rects[i].area() > 1000) {
            // Игнорируем маленькие контуры
            cv::Rect bounding_box = rects[i];
            cv::rectangle(result, bounding_box, color, 2);
        }
    }

    // cvtColor(motion_mask,result,COLOR_GRAY2BGR);
}

vector<OpticalFlowRectInfo> OpticalFlowFrameProcessor::getOpticalFlowRectsInfo(Mat &frame, const Mat &flowImage,
                                                                               const vector<Rect> &rects) {
    vector<OpticalFlowRectInfo> rectsInfo;
    for (auto rect: rects) {
        auto croppedOpticalFlow = Mat(flowImage, rect);
        auto croppedFrame = frame(rect);
        double avgX = 0;
        double avgY = 0;
        float n = 0;
        for (int y = 0; y < croppedOpticalFlow.rows; y++) {
            for (int x = 0; x < croppedOpticalFlow.cols; x++) {
                Point2f pt = croppedOpticalFlow.at<Point2f>(y, x);
                //croppedFrame.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
                if (const auto distance = std::sqrt(std::pow(pt.x, 2) + std::pow(pt.y, 2)); distance > 6) {
                    avgX += pt.x;
                    avgY += pt.y;
                    n++;
                }
            }
        }
        float ptX = 0;
        float ptY = 0;
        if (n != 0) {
            ptX = avgX / n;
            ptY = avgY / n;
        }
        auto movedRect = rect;
        int roundedPtX = -1 * cvRound(ptX);
        int roundedPtY = -1 * cvRound(ptY);
        movedRect.x += roundedPtX;
        movedRect.y += roundedPtY;
        rectsInfo.push_back(OpticalFlowRectInfo(rect, movedRect));
        if (showDebugInfo)drawOpticalFlowOnResult(croppedOpticalFlow, croppedFrame, &roundedPtX, &roundedPtY);
    }
    return rectsInfo;
}
