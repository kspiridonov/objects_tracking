#pragma once
#include <string>
#include <log4cplus/logger.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/initializer.h>
#include <cpptrace/cpptrace.hpp>
enum class LogLevel : int
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};
class Log
{
public:
    void setLogLevel(LogLevel level)
    {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
        switch (level)
        {
        case LogLevel::TRACE:
            logger.setLogLevel(log4cplus::TRACE_LOG_LEVEL);
            break;
        case LogLevel::DEBUG:
            logger.setLogLevel(log4cplus::DEBUG_LOG_LEVEL);
            break;
        case LogLevel::INFO:
            logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
            break;
        case LogLevel::WARN:
            logger.setLogLevel(log4cplus::WARN_LOG_LEVEL);
            break;
        case LogLevel::ERROR:
            logger.setLogLevel(log4cplus::ERROR_LOG_LEVEL);
            break;
        case LogLevel::FATAL:
            logger.setLogLevel(log4cplus::FATAL_LOG_LEVEL);
            break;
        }
    }
    void trace(std::string message)
    {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
        LOG4CPLUS_TRACE(logger, "[" + getPrefix() + "] " + message);
    }
    void debug(std::string message)
    {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
        LOG4CPLUS_DEBUG(logger, "[" + getPrefix() + "] " + message);
    }
    void info(std::string message)
    {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
        LOG4CPLUS_INFO(logger, "[" + getPrefix() + "] " + message);
    }
    void warn(std::string message)
    {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
        LOG4CPLUS_WARN(logger, "[" + getPrefix() + "] " + message);
    }
    void error(std::string message, const std::exception &ex)
    {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
        LOG4CPLUS_ERROR(logger, "[" + getPrefix() + "] " + message + ", ex: " + ex.what());
    }
    void fatal(std::string message)
    {
        auto logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
        LOG4CPLUS_FATAL(logger, "[" + getPrefix() + "] " + message + "stack trace: " + cpptrace::generate_trace().to_string());
    }

private:
    std::string getPrefix()
    {
        auto st = cpptrace::generate_trace();
        auto frames = st.frames;
        auto frame0 = frames.at(0);
        if (frame0.filename.find("logger.h") == std::string::npos)
        {
            return "";
        }
        auto frame = frames.at(2);
        auto fileName = frame.filename;
        std::string line = "";
        std::string col = "";
        if (frame.line.has_value())
        {
            line = std::to_string(frame.line.value());
        }
        if (frame.column.has_value())
        {
            col = std::to_string(frame.column.value());
        }
        std::string retval = fileName + "(" + line + ":" + col + ")";
        if (retval.length() > 30)
        {
            retval = "..." + retval.substr(retval.length() - 30);
        }
        return retval;
    }
};

extern Log Logger;