#include "videothread.h"

VideoThread::VideoThread(QObject *parent)
    : QThread(parent), running(false), cameraIndex(0) {}

VideoThread::~VideoThread() { stopCapture(); }

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

void VideoThread::run() {
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

      // frame = opticalFlow.calcOpticaFlow(frame);

      frame = objectDetector.detectAndDraw(frame);

      // Create QImage from OpenCV Mat
      QImage qimage(frame.data, frame.cols, frame.rows, frame.step,
                    QImage::Format_RGB888);

      // Clone the image to ensure data persists
      QImage clonedImage = qimage.copy();

      // Emit signal with the image
      emit frameCaptured(clonedImage);
    }

    // Small delay to control frame rate
    msleep(33); // ~30 FPS
  }

  if (cap.isOpened()) {
    cap.release();
  }
}
