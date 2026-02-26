#include <QApplication>
#include "logger/logger.h"
#include <exception>
class Application : public QApplication
{
public:
    using QApplication::QApplication;

    bool notify(QObject *receiver, QEvent *e) override { return QApplication::notify(receiver, e); }
};