#pragma once
#include <exception>
#include <string>
#include "core/exception/abstract_exception.h"
class QThreadException : public AbstractException
{
public:
    QThreadException(std::string message) : AbstractException(), message(message + ", stacktrace: \n" + stacktrace) {}
    const char *what() const throw() override
    {
        return message.c_str();
    }

private:
    std::string message;
};