#pragma once
#include <QObject>
#include <QThread>
#include <QString>
#include <exception>
#include "logger/logger.h"
class BaseQThread : public QThread
{
     Q_OBJECT
public:
     BaseQThread(QObject *parent, QString name);
signals:
     void exceptionOccurred(std::exception ex);

protected:
     void run() override;
     virtual void runMethod() = 0;
};