#include "LogUtil.h"
#include "IniUtil.h"
#include "FileUtil.h"
#include "EnvUtil.h"
#include "Convert.h"

// 时间库
#include <chrono>
// 宽字符输入输出库
#include <iostream>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <utility>

class LogFile {
private:
    std::wofstream file;
    std::wstring filePath;
    size_t maxSizeBytes; // 最大文件大小（字节）
    bool alwaysOverwrite; // 是否始终覆盖写入

public:
    LogFile(std::wstring path, size_t maxSize, bool overwrite)
            : filePath(std::move(path)), maxSizeBytes(maxSize), alwaysOverwrite(overwrite) {
        // 根据配置决定是覆盖模式还是追加模式
        std::ios_base::openmode mode = alwaysOverwrite ? std::ios::out : std::ios::app;
        file.open(filePath, mode);
    }

    ~LogFile() {
        if (file.is_open()) {
            file.close();
        }
    }

    bool isOpen() const {
        return file.is_open();
    }

    std::wofstream &get() {
        return file;
    }

    // 检查文件大小并在必要时重置
    void checkFileSize() {
        if (!file.is_open()) return;

        // 如果设置为始终覆盖，则不需要检查大小
        if (alwaysOverwrite) return;

        // 关闭文件以便获取大小
        file.close();

        // 获取文件大小
        std::ifstream checkFile(filePath, std::ios::binary | std::ios::ate);
        size_t fileSize = 0;
        if (checkFile.is_open()) {
            fileSize = static_cast<size_t>(checkFile.tellg());
            checkFile.close();
        }

        // 如果文件大小超过限制，则以覆盖模式重新打开
        if (fileSize >= maxSizeBytes) {
            file.open(filePath, std::ios::out); // 覆盖模式
        } else {
            file.open(filePath, std::ios::app); // 追加模式
        }
    }


    void flush() {
        if (file.is_open()) {
            file.flush();
        }
    }

    // 禁止复制
    LogFile(const LogFile &) = delete;

    LogFile &operator=(const LogFile &) = delete;
};

// 初始化静态成员
std::unique_ptr<LogFile> LogUtil::logFile = nullptr;
size_t LogUtil::maxLogSizeBytes = 10 * 1024 * 1024; // 默认 10MB
bool LogUtil::alwaysOverwrite = false; // 默认不覆盖
std::wstring LogUtil::logFileName = L"./info.log"; // 默认日志文件路径

void LogUtil::init() {
    if (!logFile) {
        // 从配置文件读取最大日志大小
        try {
            IniUtil ini(L"config.ini");
            std::wstring maxSizeStr = ini.getValue(L"Log", L"maxSizeMB");
            if (!maxSizeStr.empty()) {
                double maxSizeMB = std::stod(maxSizeStr);
                if (maxSizeMB <= 0) {
                    // 如果MaxSizeMB小于等于0，设置为始终覆盖模式
                    alwaysOverwrite = true;
                    maxLogSizeBytes = 10 * 1024 * 1024; // 设置一个默认值，虽然不会使用
                } else {
                    // 正常的大小限制模式
                    alwaysOverwrite = false;
                    // 先计算字节数（保持为 double），然后再转换为 size_t
                    double sizeInBytes = maxSizeMB * 1024 * 1024;
                    maxLogSizeBytes = static_cast<size_t>(sizeInBytes);

                    // 确保即使是小数也至少有一些大小
                    if (maxLogSizeBytes == 0 && sizeInBytes > 0) {
                        maxLogSizeBytes = 1024; // 至少设置为 1KB
                    }
                }
                std::wcout << L"日志大小设置为: " << maxLogSizeBytes << L" 字节 (来自 " << maxSizeMB << L" MB)" << std::endl;

            }

            // 替换环境变量
            std::wstring path = ini.getValue(L"Log", L"logFileName");
            logFileName = FileUtil::fixPath(EnvUtil::replaceEnvVars(path));

        } catch (...) {
            // 使用默认值
        }


        logFile = std::make_unique<LogFile>(logFileName, maxLogSizeBytes, alwaysOverwrite);
    }

}

void LogUtil::log(const std::wstring &level, const std::wstring &message) {
    // 确保 logFile 已经初始化
    if (!logFile) {
        init();
    }

    // 检查文件大小并在必要时重置
    logFile->checkFileSize();

    if (logFile && logFile->isOpen()) {
        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time);

        // 格式化时间戳
        std::wostringstream oss;
        oss << std::put_time(&now_tm, L"%Y-%m-%d %H:%M:%S");

        // 输出到控制台并立即刷新
        std::wcout << L"[" << oss.str() << L"] [" << level << L"] : " << message  << std::endl;
//        std::wcout.flush();

        // 输出日志信息到文件并立即刷新
        logFile->get() << L"[" << oss.str() << L"] [" << level << L"] : " << message  << std::endl;
//        logFile->get().flush();
    }
}

void LogUtil::info(const std::wstring &message) {
    log(L"INFO", message);
}

void LogUtil::error(const std::wstring &message) {
    log(L"ERROR", message);
}

void LogUtil::print(const std::wstring &message) {
    std::wcout << message << std::endl;
}

void LogUtil::cleanup() {
    logFile.reset();
}