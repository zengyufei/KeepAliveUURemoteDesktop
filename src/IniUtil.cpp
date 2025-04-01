#include "IniUtil.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <codecvt> // 添加 codecvt 头文件

IniUtil::IniUtil(const std::wstring& filePath) {
    parseFile(filePath);
}

void IniUtil::parseFile(const std::wstring& filePath) {

    // 使用 std::wifstream 读取文件
    std::wifstream file(filePath);
    file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t>)); // 设置 UTF-8 编码
    std::wstring line;
    std::wstring section;

    while (std::getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(L" \t"));
        line.erase(line.find_last_not_of(L" \t") + 1);

        if (line.empty() || line[0] == L';' || line[0] == L'#') {
            continue; // Skip empty lines and comments
        }

        if (line[0] == L'[' && line[line.size() - 1] == L']') {
            section = line.substr(1, line.size() - 2);
        } else {
            size_t pos = line.find(L'=');
            if (pos != std::wstring::npos) {
                std::wstring key = line.substr(0, pos);
                std::wstring value = line.substr(pos + 1);

                // Trim whitespace from key and value
                key.erase(0, key.find_first_not_of(L" \t"));
                key.erase(key.find_last_not_of(L" \t") + 1);
                value.erase(0, value.find_first_not_of(L" \t"));
                value.erase(value.find_last_not_of(L" \t") + 1);

                data[section][key] = value;
            }
        }
    }
}

std::wstring IniUtil::getValue(const std::wstring& section, const std::wstring& key) const {
    auto sectionIt = data.find(section);
    if (sectionIt != data.end()) {
        auto keyIt = sectionIt->second.find(key);
        if (keyIt != sectionIt->second.end()) {
            return keyIt->second;
        }
    }
    return L"";
}