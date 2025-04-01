#ifndef INIUTIL_H
#define INIUTIL_H

#include <string>
#include <map>

class IniUtil {
public:
    IniUtil(const std::wstring& filePath);
    std::wstring getValue(const std::wstring& section, const std::wstring& key) const;

private:
    std::map<std::wstring, std::map<std::wstring, std::wstring>> data;
    void parseFile(const std::wstring& filePath);
};

#endif // INIUTIL_H