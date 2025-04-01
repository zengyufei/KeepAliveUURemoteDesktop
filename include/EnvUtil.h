#ifndef ENVUTIL_H
#define ENVUTIL_H

#include <string>
#include <Windows.h>
#include "StrUtil.h"
#include <vector>
#include "LogUtil.h"

class EnvUtil {
public:
    // 获取环境变量的值
    static std::wstring getEnvVar(const std::wstring &var) {
        // 首先获取所需的缓冲区大小
        DWORD size = GetEnvironmentVariableW(var.c_str(), NULL, 0);
        if (size == 0) {
            return L"";  // 变量不存在
        }

        // 分配适当大小的缓冲区
        std::vector<wchar_t> buffer(size);
        if (GetEnvironmentVariableW(var.c_str(), buffer.data(), size)) {
            return std::wstring(buffer.data());
        }
        return L"";
    }

    // 替换环境变量
    static std::wstring replaceEnvVars(const std::wstring& path) {
        std::wstring result = path;
        std::wstring envVar;
        size_t startPos = 0;
        while ((startPos = StrUtil::find(result, L"%", startPos)) != std::wstring::npos) {
            size_t endPos = StrUtil::find(result, L"%", startPos + 1);
            if (endPos != std::wstring::npos) {
                envVar = StrUtil::substr(result, startPos + 1, endPos - startPos - 1);
                std::wstring envValue = getEnvVar(envVar);
                if (StrUtil::isNotBlank(envValue)) {
                    result.replace(startPos, endPos - startPos + 1, envValue);
                    startPos += envValue.length();
                } else {
                    LogUtil::error(L"无法获取环境变量: " + envVar);
                    return L"";
                }
            } else {
                break;
            }
        }
        return result;
    }
};

#endif // ENVUTIL_H