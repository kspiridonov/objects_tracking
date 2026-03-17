#include "videothread.h"
#include "logger/logger.h"
#include "core/qthread/q_thread_exception.h"
#include "core/rect/object_rect/object_rect.h"

VideoThread::VideoThread(QObject *parent, QString name) : BaseQThread(parent, name),
                                                          FrameProcessorListener<ObjectRect>("VideoThread"),
                                                          running(false), cameraIndex(0) {
    objectRectsFrameProcessor.addListener(this);
}

VideoThread::~VideoThread() {
    stopCapture();
    objectRectsFrameProcessor.removeListener(this);
}

void VideoThread::startCapture(int cameraIndex) {
    QMutexLocker locker(&mutex);
    if (!isRunning()) {
        this->cameraIndex = cameraIndex;
        start();
        running = true;
    }
}

void VideoThread::startCapture(const QString &path) {
    QMutexLocker locker(&mutex);
    if (!isRunning()) {
        cameraIndex = NO_CAM;
        this->path = path;
        start();
        running = true;
    }
}

void VideoThread::stopCapture() {
    {
        QMutexLocker locker(&mutex);
        running = false;
    }
    wait();
    if (cap.isOpened()) {
        cap.release();
    }
}

bool VideoThread::isRunning() const { return running; }

void VideoThread::runMethod() {
    {
        QMutexLocker locker(&mutex);
        if (cameraIndex != NO_CAM) {
            cap.open(cameraIndex);
        } else {
            cap.open(path.toStdString());
        }
    }
    if (!cap.isOpened()) {
        QString res = "camera";
        if (cameraIndex == NO_CAM) {
            res = "file: " + path;
        }
        emit errorOccurred("Failed to open" + res);
        return;
    }
    cv::Mat frame;
    while (true) {
        {
            QMutexLocker locker(&mutex);
            if (!running)
                break;
        }
        if (!cap.read(frame)) {
            emit errorOccurred("Failed to capture frame");
            break;
        }

        if (!frame.empty()) {
            // Convert BGR to RGB
            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
            objectRectsFrameProcessor.processFrame(frame);
            //frameProcessed(frame,vector<ObjectRect>());
        }

        // Small delay to control frame rate
        //msleep(1); // ~30 FPS
    }

    if (cap.isOpened()) {
        cap.release();
    }
}

void VideoThread::frameProcessed(Mat &frame, const vector<ObjectRect> &list) {
    //  Create QImage from OpenCV Mat
    drawRects(frame, list);
    QImage qimage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    // Clone the image to ensure data persists
    QImage clonedImage = qimage.copy();
    // Emit signal with the image
    emit frameCaptured(clonedImage);
}

void VideoThread::drawRects(Mat &frame, vector<ObjectRect> rects) {
    for (size_t i = 0; i < rects.size(); i++) {
        Rect bounding_box = rects[i].getRect();
        auto numDetections = rects[i].getNumDetections();
        DetectionRectType detectionType = rects[i].getType();
        string label = EnumUtil::toString(detectionType);
        if (numDetections > 1) {
            label +=". Num:" + to_string(numDetections);
        }
        rectangle(frame, bounding_box, cv::Scalar(0, 255, 0), 2);
        putText(frame, label,
                cv::Point(bounding_box.x, bounding_box.y - 10),
                cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0, 0, 0), 2, 0);
    }
}
