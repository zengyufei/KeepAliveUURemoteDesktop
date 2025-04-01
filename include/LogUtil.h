#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "StrUtil.h"

class LogFile {
private:
    std::wofstream file;

public:
    LogFile(const std::wstring& path) {
        file.open(path, std::ios::app);
    }

    ~LogFile() {
        if (file.is_open()) {
            file.close();
        }
    }

    bool isOpen() const { return file.is_open(); }
    std::wofstream& get() { return file; }

    // 禁止复制
    LogFile(const LogFile&) = delete;
    LogFile& operator=(const LogFile&) = delete;
};

class LogUtil {
private:
    // 使用 inline 关键字来避免多重定义
    static inline std::unique_ptr<LogFile> logFile;

public:
    static void init() {
        if (!logFile) {
            logFile = std::make_unique<LogFile>(L"info.log");
        }
    }

    static void info(const std::wstring& message) {
        log(L"info", message);
    }

    static void error(const std::wstring& message) {
        log(L"error", message);
    }

    static void cleanup() {
        logFile.reset();
    }

private:
    static void log(const std::wstring& level, const std::wstring& message) {
        // 确保 logFile 已经初始化
        if (!logFile) {
            init();
        }

        if (logFile && logFile->isOpen()) {
            // 获取当前时间
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);
            std::tm now_tm = *std::localtime(&now_time);

            // 格式化时间戳
            std::wostringstream oss;
            oss << std::put_time(&now_tm, L"%Y-%m-%d %H:%M:%S");

            std::wcout << L"[" << oss.str() << L"] [" << level << L"] : " << message << std::endl;
            // 输出日志信息
            logFile->get() << L"[" << oss.str() << L"] [" << level << L"] : " << message << std::endl;
        }
    }
};

#endif // LOGUTIL_H