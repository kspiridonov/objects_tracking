#pragma once

#include <QImage>
#include <QMutex>
#include <QObject>
#include <QString>
#include <opencv2/opencv.hpp>
#include "core/qthread/base_thread.h"
#include "core/rect/object_rect/object_rect.h"
#include "frame_processors/base_frame_processor/frame_processor_listener.h"
#include "frame_processors/movement_rects_frame_processor/movement_rects_frame_processor.h"
#include "frame_processors/object_rects_frame_processor/object_rects_frame_processor.h"

class VideoThread : public BaseQThread, FrameProcessorListener<ObjectRect> {
    Q_OBJECT

public:
    VideoThread(QObject *parent, QString name);

    ~VideoThread();

    void startCapture(int cameraIndex = 0);

    void startCapture(const QString &path);

    void stopCapture();

    bool isRunning() const;

    void frameProcessed(Mat &frame, const vector<ObjectRect> &list) override;

signals:
    void frameCaptured(const QImage &frame);

    void errorOccurred(const QString &error);

protected:
    void runMethod() override;
    void drawRects(Mat &frame, vector<ObjectRect> rects);

private:
    int const NO_CAM = -1;
    cv::VideoCapture cap;
    bool running = false;
    QMutex mutex;
    int cameraIndex;
    QString path;
    ObjectRectsFrameProcessor objectRectsFrameProcessor;
};
