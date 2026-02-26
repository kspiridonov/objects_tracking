#include "application.h"
#include "logger/logger.h"
#include "widgets/main_window/mainwindow.h"
void on_terminate()
{
    Logger.fatal("on_terminate"); // перепосылка сигнала
    exit(3); // выход из программы. Если не сделать этого, то обработчик будет вызываться бесконечно.
}

int main(int argc, char *argv[]) {
    std::set_terminate(on_terminate);
    log4cplus::Initializer initializer;
    log4cplus::BasicConfigurator config;
    config.configure();
    Logger.setLogLevel(LogLevel::TRACE);
    Application app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
