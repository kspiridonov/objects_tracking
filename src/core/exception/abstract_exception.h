#pragma once
#include <cpptrace/cpptrace.hpp>
#include <exception>
#include <string>
class AbstractException : public std::exception
{
public:
    AbstractException() { stacktrace = cpptrace::generate_trace().to_string(); }
    const char *what() const throw() override { return stacktrace.c_str(); }

protected:
    std::string stacktrace;
};