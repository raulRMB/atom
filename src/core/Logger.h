//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_LOGGER_H
#define ATOM_LOGGER_H

namespace atom
{
    enum class LogLevel
    {
        Info = 0,
        Warning,
        Error
    };

    void Log(LogLevel logLevel, const char* format, ...);
    void LogInfo(const char* format, ...);
    void LogWarning(const char* format, ...);
    void LogError(const char* format, ...);
}


#endif //ATOM_LOGGER_H
