#include "videothread.h"
#include "logger/logger.h"
#include "core/qthread/q_thread_exception.h"
VideoThread::VideoThread(QObject *parent, QString name) : BaseQThread(parent, name), FrameProcessorListener<Area>("VideoThread"), running(false), cameraIndex(0)
{
  movedAreasProcessor.addListener(this);
}

VideoThread::~VideoThread()
{
  stopCapture();
  movedAreasProcessor.removeListener(this);
}

void VideoThread::startCapture(int cameraIndex)
{
  QMutexLocker locker(&mutex);
  if (!isRunning())
  {
    this->cameraIndex = cameraIndex;
    start();
    running = true;
  }
}

void VideoThread::startCapture(const QString &path)
{
  QMutexLocker locker(&mutex);
  if (!isRunning())
  {
    cameraIndex = NO_CAM;
    this->path = path;
    start();
    running = true;
  }
}

void VideoThread::stopCapture()
{
  {
    QMutexLocker locker(&mutex);
    running = false;
  }
  wait();
  if (cap.isOpened())
  {
    cap.release();
  }
}

bool VideoThread::isRunning() const { return running; }

void VideoThread::runMethod()
{
  {
    QMutexLocker locker(&mutex);
    if (cameraIndex != NO_CAM)
    {
      cap.open(cameraIndex);
    }
    else
    {
      cap.open(path.toStdString());
    }
  }
  if (!cap.isOpened())
  {
    QString res = "camera";
    if (cameraIndex == NO_CAM)
    {
      res = "file: " + path;
    }
    emit errorOccurred("Failed to open" + res);
    return;
  }
  cv::Mat frame;
  while (true)
  {
    {
      QMutexLocker locker(&mutex);
      if (!running)
        break;
    }
    if (!cap.read(frame))
    {
      emit errorOccurred("Failed to capture frame");
      break;
    }

    Logger.trace("Get new fame");

    if (!frame.empty())
    {
      // Convert BGR to RGB
      cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

      movedAreasProcessor.processFrame(frame);
    }

    // Small delay to control frame rate
    msleep(33); // ~30 FPS
  }

  if (cap.isOpened())
  {
    cap.release();
  }
}

void VideoThread::frameProcessed(Mat &frame, vector<Area> list)
{
  // frame = opticalFlow.calcOpticaFlow(frame);
  //  Create QImage from OpenCV Mat
  QImage qimage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

  // Clone the image to ensure data persists
  QImage clonedImage = qimage.copy();
  // Emit signal with the image
  emit frameCaptured(clonedImage);
}
