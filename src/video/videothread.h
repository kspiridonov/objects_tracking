#pragma once

#include "video/object_detector/ObjectDetector.h"
#include <QImage>
#include <QMutex>
#include <QObject>
#include <QString>
#include <opencv2/opencv.hpp>
#include "core/qthread/base_thread.h"
#include "frame_processors/base_frame_processor/frame_processor_listener.h"
#include "core/movement_area/movement_area.h"
#include "frame_processors/movement_areas_frame_processor/movement_areas_frame_processor.h"
class VideoThread : public BaseQThread, FrameProcessorListener<MovementArea>
{
  Q_OBJECT
public:
  VideoThread(QObject *parent, QString name);
  ~VideoThread();
  void startCapture(int cameraIndex = 0);
  void startCapture(const QString &path);
  void stopCapture();
  bool isRunning() const;
  void frameProcessed(Mat &frame, vector<MovementArea> list) override;
signals:
  void frameCaptured(const QImage &frame);
  void errorOccurred(const QString &error);

protected:
  void runMethod() override;
  void drawAreas(Mat &frame,vector<MovementArea> areas);

private:
  int const NO_CAM = -1;
  cv::VideoCapture cap;
  bool running = false;
  QMutex mutex;
  int cameraIndex;
  QString path;
  MovementAreasFrameProcessor movedAreasProcessor;
};
