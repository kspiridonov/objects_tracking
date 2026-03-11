#pragma once
#include <exception>
#include <string>
#include "abstract_exception.h"
class StandardException : public AbstractException
{
public:
    StandardException(std::string message) : AbstractException(), message(message + ", stacktrace: \n" + stacktrace) {}
    const char *what() const throw() override
    {
        return message.c_str();
    }

private:
    std::string message;
};