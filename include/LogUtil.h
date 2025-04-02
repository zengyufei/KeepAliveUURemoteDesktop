#ifndef LOGUTIL_H
#define LOGUTIL_H
#pragma once

#include <memory>
#include <string>

// 前向声明
class LogFile;

class LogUtil {
private:
    static std::unique_ptr<LogFile> logFile;
    static size_t maxLogSizeBytes; // 最大日志文件大小（字节）
    static bool alwaysOverwrite; // 是否始终覆盖写入
    static std::wstring logFileName; // 默认的日志文件路径

    static void init();
    static void log(const std::wstring& level, const std::wstring& message);

public:
    static void info(const std::wstring& message);
    static void error(const std::wstring& message);
    static void print(const std::wstring& message);
    static void cleanup();
};

#endif // LOGUTIL_H