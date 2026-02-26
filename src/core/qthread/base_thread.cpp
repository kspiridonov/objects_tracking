#include "base_thread.h"
#include "core/qthread/q_thread_exception.h"
BaseQThread::BaseQThread(QObject *parent, QString name) : QThread(parent)
{
    this->setObjectName(name);
}
void BaseQThread::run()
{
    try
    {
        runMethod();
    }
    catch (std::exception &ex)
    {
        emit exceptionOccurred(ex);
        auto threadId = QThread::currentThreadId();
        Logger.error("Error occured in thread: " + this->objectName().toStdString(), ex);
    }
}