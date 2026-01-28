#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "video/videothread.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
private slots:
  void onStartClicked();
  void onStopClicked();
  void updateFrame(const QImage &frame);
  void onError(const QString &error);

private:
  void setupUI();
  void setupConnections();

  QLabel *videoLabel;
  QPushButton *startButton;
  QPushButton *stopButton;
  QLabel *statusLabel;

  VideoThread *videoThread;
};

#endif // MAINWINDOW_H
