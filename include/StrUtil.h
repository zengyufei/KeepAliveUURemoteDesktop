#ifndef STRUTIL_H
#define STRUTIL_H

//#include <string>
#include <cctype>    // for std::isspace
#include <codecvt>
//#include <locale>

#include <iostream>
#include <algorithm> // for std::equal
#include <vector>    // for std::vector
#include <cwctype>

using namespace std;

class StrUtil {
public:
    // Convert between UTF-8 and wide strings
    static std::wstring convertUtf8(const std::string &str) {
        // 确保路径使用UTF-8编码
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring widePath = converter.from_bytes(str);
        return widePath;
    }

    static std::string convertWstringToString(const std::wstring& wstr) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }

    static bool isBlank(const std::wstring &str) {
        return str.empty() || std::all_of(str.begin(), str.end(), ::iswspace);
    }

    static bool isNotBlank(const std::wstring &str) {
        return !isBlank(str);
    }

    static bool equals(const std::wstring &str1, const std::wstring &str2) {
        return str1 == str2;
    }

    static bool equalsIgnoreCase(const std::wstring &str1, const std::wstring &str2) {
        if (str1.length() != str2.length()) {
            return false;
        }
        return std::equal(str1.begin(), str1.end(), str2.begin(),
                          [](wchar_t a, wchar_t b) {
                              return std::towlower(a) == std::towlower(b);
                          });
    }

    // 替换字符串中的子字符串
    static std::wstring replace(const std::wstring &str, const std::wstring &from, const std::wstring &to) {
        std::wstring result = str;
        size_t start_pos = 0;
        while ((start_pos = result.find(from, start_pos)) != std::wstring::npos) {
            result.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return result;
    }

    // 获取子字符串
    static std::wstring substr(const std::wstring &str, size_t pos, size_t len = std::wstring::npos) {
        return str.substr(pos, len);
    }

    // 查找子字符串的位置
    static size_t find(const std::wstring &str, const std::wstring &substr, size_t pos = 0) {
        return str.find(substr, pos);
    }

    // 将字符串向量中的元素用指定的分隔符连接成一个字符串
    static std::wstring join(const std::vector<std::wstring> &parts, const std::wstring &delimiter) {
        if (parts.empty()) {
            return L"";
        }
        std::wstring result = parts[0];
        for (size_t i = 1; i < parts.size(); ++i) {
            result += delimiter + parts[i];
        }
        return result;
    }

    // 去除字符串两端的空白字符
    static std::wstring trim(const std::wstring &str) {
        if (str.empty()) {
            return str;
        }
        std::wstring result = trimStart(str);
        return trimEnd(result);
    }

    // 去除字符串左侧的空白字符
    static std::wstring trimStart(const std::wstring &str) {
        if (isBlank(str)) {
            return str;
        }
        std::wstring result = str;
        result.erase(result.begin(),
                     std::find_if(
                             result.begin(),
                             result.end(),
                             [](wchar_t ch) {
                                 return !std::iswspace(ch);
                             }));
        return result;
    }

    // 去除字符串右侧的空白字符
    static std::wstring trimEnd(const std::wstring &str) {
        if (str.empty()) {
            return str;
        }
        auto it = std::find_if(
                str.rbegin(),
                str.rend(),
                [](wchar_t ch) {
                    return !std::iswspace(ch);
                });
        std::wstring result = str;
        result.erase(it.base(), result.end());
        return result;
    }
};

#endif // STRUTIL_H