#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <windows.h>
#include "StrUtil.h" // 自定义下载工具头文件
#include "EnvUtil.h" // 自定义环境工具头文件
#include "LogUtil.h" // 添加日志工具头文件

// 文件操作 RAII 包装类
template<typename StreamType>
class FileGuard {
private:
    StreamType file;
    std::wstring path;

public:
    FileGuard(const std::wstring& path, std::ios_base::openmode mode) : path(path) {
        file.open(path, mode);
    }

    ~FileGuard() {
        if (file.is_open()) {
            file.close();
        }
    }

    bool isOpen() const { return file.is_open(); }
    StreamType& get() { return file; }

    // 禁止复制
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;
};

class FileUtil {
public:
    // 修改返回类型为 int
    static int fileSize(const std::wstring &path) {
        if (StrUtil::isNotBlank(path) && FileUtil::exists(path)) {
            // 使用 std::ifstream 特化的 FileGuard
            FileGuard<std::wifstream> file(path, std::ios::binary | std::ios::ate);
            if (file.isOpen()) {
                std::streamsize fileSize = file.get().tellg();
                return static_cast<int>(fileSize);
            }
        }
        return 0;
    }

    static bool exists(const std::wstring &path) {
        return StrUtil::isNotBlank(path) && std::filesystem::exists(path);
    }

    static bool notExists(const std::wstring &path) {
        return !exists(path);
    }

    static std::wstring readString(const std::wstring &path) {
        std::wstring content;
        // 使用 std::ifstream 特化的 FileGuard
        FileGuard<std::wifstream> file(path, std::ios::in);
        if (file.isOpen()) {
            std::getline(file.get(), content);
            return content;
        } else {
            LogUtil::error(L"无法打开文件进行读取: " + path);
        }
        return content;
    }

    static bool mkdir(const std::wstring &path) {
        // 创建下载目录
        if (notExists(path)) {
            std::filesystem::path dirPath(path);
            std::filesystem::create_directories(dirPath);
            return true;
        }
        return false;
    }

    static bool canRead(const std::wstring &path) {
        // 使用 RAII 模式管理文件资源
        FileGuard<std::wifstream> file(path, std::ios::in);
        return file.isOpen();
    }

    // 修复路径
    static std::wstring fixPath(const std::wstring &path) {
        if (StrUtil::isBlank(path)) {
            return path;
        }

        std::wstring result = path;
        // 将 \ 转换为 /
        result = StrUtil::replace(result, L"\\", L"/");

        // 去除左边空格
        result = StrUtil::trimStart(result);
        // 统一使用 / 作为路径分隔符，并去除多个连续的 /
        result = StrUtil::replace(result, L"//", L"/");
        while (StrUtil::find(result, L"//") != std::wstring::npos) {
            result = StrUtil::replace(result, L"//", L"/");
        }

        // 处理 ~ 表示用户主目录的情况
        if (result[0] == L'~') {
            std::wstring homeDir = getHomeDir();
            result = homeDir + StrUtil::substr(result, 1);
        }

        // 处理 . 和 ..
        std::vector<std::wstring> parts;
        std::wstring part;
        std::wistringstream iss(result);
        while (std::getline(iss, part, L'/')) {
            if (part == L"." || part.empty()) {
                continue;
            } else if (part == L"..") {
                if (!parts.empty()) {
                    parts.pop_back();
                } else {
                    // 如果 .. 多于已有路径，直接返回根路径
                    result = L"/";
                    break;
                }
            } else {
                parts.push_back(part);
            }
        }

        // 重新构建路径
        if (result != L"/") {
            result = StrUtil::join(parts, L"/");
        }

        return result;
    }

    // 拼接路径
    template<typename... Args>
    static std::wstring join(Args... args) {
        std::vector<std::wstring> paths = {args...};
        return join(paths);
    }

    // 删除文件
    static bool removeFile(const std::wstring &path) {
        if (StrUtil::isNotBlank(path)) {
            try {
                if (!std::filesystem::remove(path)) {
                    LogUtil::error(L"无法删除文件: " + path);
                    return false;
                } else {
                    LogUtil::info(L"文件已删除: " + path);
                    return true;
                }
            } catch (const std::filesystem::filesystem_error& e) {
                LogUtil::error(L"删除文件时发生错误: " + path + L" - " + StrUtil::convertStringToWstring(e.what()));
                return false;
            }
        }
        return false;
    }

    static bool move(const std::wstring &source, const std::wstring &destination) {
        if (StrUtil::isBlank(source) || StrUtil::isBlank(destination)) {
            LogUtil::error(L"源路径或目标路径为空: source=" + source + L", destination=" + destination);
            return false;
        }

        try {
            // 检查源文件是否存在
            if (!exists(source)) {
                LogUtil::error(L"源文件不存在: " + source);
                return false;
            }

            // 执行移动操作
            std::filesystem::rename(source, destination);

            // 验证目标文件是否存在
            if (!exists(destination)) {
                LogUtil::error(L"目标文件未创建: " + destination);
                return false;
            }

            // 删除源文件（如果仍然存在）
            if (exists(source)) {
                removeFile(source);
            }

            LogUtil::info(L"文件移动成功: source=" + source + L", destination=" + destination);
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            LogUtil::error(L"移动文件时发生错误: source=" + source + L", destination=" + destination + L" - " + StrUtil::convertStringToWstring(e.what()));
            return false;
        }
    }

private:
    // 获取用户主目录
    static std::wstring getHomeDir() {
        std::wstring home = EnvUtil::getEnvVar(L"HOME");
        if (!home.empty()) {
            return home;
        }

        home = EnvUtil::getEnvVar(L"USERPROFILE");
        if (!home.empty()) {
            return home;
        }

        return L"/"; // 默认根路径
    }

    // 私有函数：拼接路径
    static std::wstring join(const std::vector<std::wstring>& paths) {
        std::wstring result;
        for (size_t i = 0; i < paths.size(); ++i) {
            if (i > 0 && !result.empty() && !paths[i].empty() &&
                result.back() != L'/' && paths[i].front() != L'/') {
                result += L'/';
            }
            result += paths[i];
        }
        return fixPath(result);
    }
};

#endif // FILEUTIL_H







