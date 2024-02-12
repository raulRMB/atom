//
// Created by Raul Romero on 2024-01-13.
//

#include <cstdarg>
#include <cstdio>
#include <string>
#include <iostream>
#include "Logger.h"
#include <cassert>

namespace atom
{
    void Log(LogLevel logLevel, const char* format, ...)
    {
        std::string outString;
        switch(logLevel)
        {
            case LogLevel::Info:
                outString = "[INFO]: ";
                break;
            case LogLevel::Warning:
                outString = "[WARNING]: ";
                break;
            case LogLevel::Error:
                outString = "[ERROR]: ";
                break;
        }

        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsprintf_s(buffer, format, args);
        va_end(args);
        outString += buffer;
        std::cout << outString << std::endl;
    }

    void LogInfo(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsprintf_s(buffer, format, args);
        va_end(args);
        std::string outString = "[INFO]: ";
        outString += buffer;
        std::cout << outString << std::endl;
    }

    void LogWarning(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsprintf_s(buffer, format, args);
        va_end(args);
        std::string outString = "[WARNING]: ";
        outString += buffer;
        std::cout << outString << std::endl;
    }

    void LogError(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        char* buffer = new char[16384];
        vsprintf_s(buffer, 16384, format, args);
        va_end(args);
        std::string outString = "[ERROR]: ";
        outString += buffer;
        std::cout << outString << std::endl;
        assert(false);
        delete[] buffer;
    }
}