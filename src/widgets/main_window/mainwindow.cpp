#include "mainwindow.h"
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}, videoThread(nullptr) {
  setupUI();
  setupConnections();

  // Initialize video thread
  videoThread = new VideoThread(this);
}

MainWindow::~MainWindow() {
  if (videoThread) {
    videoThread->stopCapture();
    delete videoThread;
  }
}

void MainWindow::setupUI() {
  // Central widget
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Main layout
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  // Video display label
  videoLabel = new QLabel(this);
  videoLabel->setMinimumSize(640, 480);
  videoLabel->setAlignment(Qt::AlignCenter);
  videoLabel->setStyleSheet("border: 2px solid black;");
  videoLabel->setText("No video");
  mainLayout->addWidget(videoLabel);

  // Status label
  statusLabel = new QLabel("Ready", this);
  mainLayout->addWidget(statusLabel);

  // Button layout
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  startButton = new QPushButton("Start Video", this);
  stopButton = new QPushButton("Stop Video", this);
  stopButton->setEnabled(false);

  buttonLayout->addWidget(startButton);
  buttonLayout->addWidget(stopButton);
  mainLayout->addLayout(buttonLayout);

  // Set window properties
  setWindowTitle("OpenCV Video in QThread - Qt C++");
  resize(800, 600);
}

void MainWindow::setupConnections() {
  connect(startButton, &QPushButton::clicked, this,
          &MainWindow::onStartClicked);
  connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
}

void MainWindow::onStartClicked() {
  if (videoThread) {
    videoThread->startCapture(0); // 0 for default camera

    // Connect signals after thread starts
    connect(videoThread, &VideoThread::frameCaptured, this,
            &MainWindow::updateFrame, Qt::QueuedConnection);
    connect(videoThread, &VideoThread::errorOccurred, this,
            &MainWindow::onError, Qt::QueuedConnection);

    startButton->setEnabled(false);
    stopButton->setEnabled(true);
    statusLabel->setText("Capturing...");
  }
}

void MainWindow::onStopClicked() {
  if (videoThread) {
    videoThread->stopCapture();

    // Disconnect signals
    disconnect(videoThread, &VideoThread::frameCaptured, this,
               &MainWindow::updateFrame);
    disconnect(videoThread, &VideoThread::errorOccurred, this,
               &MainWindow::onError);

    videoLabel->setText("Stopped");
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
    statusLabel->setText("Stopped");
  }
}

void MainWindow::updateFrame(const QImage &frame) {
  // Scale image to fit label while maintaining aspect ratio
  QPixmap pixmap = QPixmap::fromImage(frame);
  if (!pixmap.isNull()) {
    pixmap = pixmap.scaled(videoLabel->size(), Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
    videoLabel->setPixmap(pixmap);
  }
}

void MainWindow::onError(const QString &error) {
  QMessageBox::warning(this, "Error", error);
  onStopClicked();
  statusLabel->setText("Error: " + error);
}
