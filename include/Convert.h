#ifndef CONVERT_H
#define CONVERT_H

#include "StrUtil.h" // 自定义下载工具头文件
#include "LogUtil.h"

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

    static int toInt(const std::wstring& str) {
        return std::stoi(str);
    }

    static int toInt(const std::wstring& str, int defaultValue) {
        if (StrUtil::isBlank(str)) {
            return defaultValue;
        }
        try {
            return std::stoi(str);
        } catch (const std::invalid_argument& e) {
            LogUtil::error(L"无效的整数字符串: " + str);
            return defaultValue; // 或者返回其他默认值
        } catch (const std::out_of_range& e) {
            LogUtil::error(L"整数字符串超出范围: " + str);
            return defaultValue; // 或者返回其他默认值
        }
    }
};

#endif // CONVERT_H