#ifndef CONSTANT_H
#define CONSTANT_H

#include <string>

class Constant {
public:
    // 配置文件相关常量
    static const std::wstring CONFIG_FILE_NAME;
    static const std::wstring SETTINGS;

    // 配置键名常量
    static const std::wstring KEY_PATH;
    static const std::wstring KEY_PROGRAM_NAME;
    static const std::wstring KEY_SECONDS;
};

#endif // CONSTANT_H
