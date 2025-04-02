#ifndef CONVERT_H
#define CONVERT_H
#pragma once

#include <optional>
#include <sstream>  // 确保包含这个头文件
#include <iomanip>  // 确保包含这个头文件

#include "StrUtil.h" // 自定义下载工具头文件
#include "LogUtil.h"

class Convert {
public:
    static std::wstring toStr(const wchar_t* str) {
        return StrUtil::isBlank(str) ? std::wstring() : std::wstring(str);
    }

    static std::wstring toStr(const wchar_t* str, const std::wstring& defaultStr) {
        return StrUtil::isBlank(str) ? defaultStr : std::wstring(str);
    }

    // 将 int 转换为 std::wstring
    static std::wstring toStr(int value) {
        return std::to_wstring(value);
    }


    // 兼容性方法 - 从 char* 转换
    static std::wstring toStr(const char* str) {
        return str ? StrUtil::convertStringToWstring(std::string(str)) : std::wstring();
    }



    /**
     * 将 size_t 转换为字符串
     * @param value 要转换的 size_t 值
     * @return 转换后的字符串
     */
    static std::wstring toStr(size_t value) {
        return std::to_wstring(value);
    }


    /**
     * 将 double 转换为字符串
     * @param value 要转换的 double 值
     * @param precision 小数点后的精度（默认为2位）
     * @return 转换后的字符串
     */
    static std::wstring toStr(double value, int precision = 2) {
        std::wostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }

    static std::optional<int> toInt(const std::wstring& str) {
        try {
            return std::stoi(str);
        } catch (const std::invalid_argument& e) {
            return std::nullopt; // 转换失败，返回空值
        } catch (const std::out_of_range& e) {
            return std::nullopt; // 转换失败，返回空值
        }
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