#pragma once

#include <iostream>
#include <exception>
#include <string>

class InvalidMove : public std::exception
{
private:
    std::string message;

public:
    InvalidMove(const std::string &msg) : message(msg) {}

    const char *what() const noexcept override
    {
        return message.c_str();
    }
};
