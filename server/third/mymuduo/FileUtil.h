#pragma once

#include "noncopyable.h"
#include <string>
#include <sys/types.h>

using namespace std;

namespace FileUtil
{
    class ReadSmallFile : noncopyable
    {
    public:
        ReadSmallFile(string filename);
        ~ReadSmallFile();

        int readToString(int maxSize,
                         string *content,
                         int64_t *fileSize,
                         int64_t *modifyTime,
                         int64_t *createTime);
        int readToBuffer(int *size);

        const char *buffer() const { return buf_; }

        static const int kBufferSize = 64 * 1024;

    private:
        int fd_;
        int err_;
        char buf_[kBufferSize];
    };

    int readFile(string filename,
                 int maxSize,
                 string *content,
                 int64_t *fileSize = nullptr,
                 int64_t *modifyTime = nullptr,
                 int64_t *createTime = nullptr);
    
    class AppendFile : noncopyable
    {
    public:
        explicit AppendFile(string filename);

        ~AppendFile();

        void append(const char *logline, size_t len);

        void flush();

        off_t writtenBytes() const { return writtenBytes_; }

    private:
        size_t write(const char *logline, size_t len);

        FILE *fp_;
        char buffer_[64 * 1024];
        off_t writtenBytes_;
    };
}