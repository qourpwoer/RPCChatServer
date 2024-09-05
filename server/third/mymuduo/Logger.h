// #pragma once

// #include <string>

// #include "noncopyable.h"
// // LOG_INFO("%s%d", arg1, arg2)
// #define LOG_INFO(logmsgFormat, ...) \
//     do \
//     { \
//         Logger &logger = Logger::instance();\
//         logger.setLogLevel(INFO);\
//         char buf[1024] = {0};\
//         snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
//         logger.log(buf);\
//     } while(0)
// #define LOG_ERROR(logmsgFormat, ...) \
//     do \u
//     { \
//         Logger &logger = Logger::instance();\
//         logger.setLogLevel(ERROR);\
//         char buf[1024] = {0};\
//         snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
//         logger.log(buf);\
//     } while(0)
// #define LOG_FATAL(logmsgFormat, ...) \
//     do \
//     { \
//         Logger &logger = Logger::instance();\
//         logger.setLogLevel(FATAL);\
//         char buf[1024] = {0};\
//         snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
//         logger.log(buf);\
//         exit(-1);\
//     } while(0)
// #ifdef MUDEBUG
// #define LOG_DEBUG(logmsgFormat, ...) \
//     do \
//     { \
//         Logger &logger = Logger::instance();\
//         logger.setLogLevel(DEBUG);\
//         char buf[1024] = {0};\
//         snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);\
//         logger.log(buf);\
//     } while(0)
// #else
//     #define LOG_DEBUG(logmsgFormat, ...)
// #endif

// // 定义日志的级别 INFO ERROR FATAL DEBUG
// enum LogLevel
// {
//     INFO, // 普通信息
//     ERROR, // 错误信息
//     FATAL, //core信息
//     DEBUG, //调试信息
// };

// // 输出一个日志类
// class Logger : noncopyable
// {
// public:
//     //  获取日志唯一的对象
//     static Logger& instance();
//     // 设置日志级别
//     void setLogLevel(int level);
//     //  写日志
//     void log(std::string msg);
// private:
//     Logger() {}
//     int logLevel_;
// };

#pragma once

#include "LogStream.h"
#include "Timestamp.h"

class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    class SourceFile
    {
    public:
        template <int N>
        SourceFile(const char (&arr)[N])
            : data_(arr),
              size_(N - 1)
        {
            const char *slash = strchr(data_, '/');
            if (slash)
            {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char *filename)
            : data_(filename)
        {
            const char *slash = strchr(filename, '/');
            if (slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char *data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char *func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream &stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char *msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);
    // static void setTimeZone(const TimeZone &tz);

    class Impl
    {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile &file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

private:
    Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

#define LOG_TRACE                            \
    if (Logger::logLevel() <= Logger::TRACE) \
    Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG                            \
    if (Logger::logLevel() <= Logger::TRACE) \
    Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO                            \
    if (Logger::logLevel() <= Logger::INFO) \
    Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

const char *strerror_tl(int saveErrno);

#define CHECK_NOTNULL(val) \
    ::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T *CheckNotNull(Logger::SourceFile file, int line, const char *names, T *ptr)
{
    if (ptr == NULL)
    {
        Logger(file, line, Logger::FATAL).stream() << names;
    }
    return ptr;
}