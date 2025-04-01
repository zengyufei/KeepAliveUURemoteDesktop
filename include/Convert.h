#ifndef CONVERT_H
#define CONVERT_H

#include "StrUtil.h" // 自定义下载工具头文件

class Convert {
public:
    static std::wstring toStr(const wchar_t* str) {
        return StrUtil::isBlank(str) ? std::wstring() : std::wstring(str);
    }

    static std::wstring toStr(const wchar_t* str, const std::wstring defaultStr) {
        return StrUtil::isBlank(str) ? defaultStr : std::wstring(str);
    }

    // 将 int 转换为 std::wstring
    static std::wstring toStr(int value) {
        return std::to_wstring(value);
    }

    // 兼容性方法 - 从 char* 转换
    static std::wstring toStr(const char* str) {
        return str ? StrUtil::convertUtf8(std::string(str)) : std::wstring();
    }

};

#endif // CONVERT_H