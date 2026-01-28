#pragma once

#include <QImage>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
class VideoThread : public QThread {
  Q_OBJECT
public:
  VideoThread(QObject *parent = nullptr);
  ~VideoThread();
  void startCapture(int cameraIndex = 0);
  void startCapture(const QString &path);
  void stopCapture();
  bool isRunning() const;
signals:
  void frameCaptured(const QImage &frame);
  void errorOccurred(const QString &error);

protected:
  void run() override;

private:
  int const NO_CAM = -1;
  cv::VideoCapture cap;
  bool running = false;
  QMutex mutex;
  int cameraIndex;
  QString path;
};
