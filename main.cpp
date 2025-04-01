#include <string>
#include <windows.h>

using namespace std;

#include "Convert.h"
#include "IniUtil.h"
#include "LogUtil.h" // 引入 LogUtil 头文件
#include "FileUtil.h" // 引入文件操作工具头文件
#include "PidUtil.h" // 引入进程管理工具头文件
#include "StrUtil.h"
#include "constant/Constant.h"
#include "EnvUtil.h" // 引入环境变量工具头文件
#include <thread>
#include <chrono>


bool executeFile(const std::wstring &programPath);

void run(int argc, char *argv[]);


[[noreturn]] void foo(const wstring &path, const wstring &programName, int seconds);

int main(int argc, char *argv[]) {
    // 系统级编码设置
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // C++ 层面编码设置
    std::locale::global(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale());
    std::cout.imbue(std::locale());

    run(argc, argv);

    return 0;
}

void run(int argc, char *argv[]) {

    LogUtil::info(L"程序启动");

    // 使用RAII模式确保在函数退出时清理资源
    class LogCleanup {
    public:
        ~LogCleanup() { LogUtil::cleanup(); }
    } logCleanup;

    // 检查配置文件是否存在
    if (!FileUtil::exists(Constant::CONFIG_FILE_NAME)) {
        LogUtil::error(L"配置文件 config.ini 不存在");
        return;
    }

    // 【Settings】
    std::wstring path;
    std::wstring programName;
    std::wstring secondsStr;

    // 获取配置文件
    IniUtil ini(Constant::CONFIG_FILE_NAME);

    // 【Settings】
    path = ini.getValue(Constant::SETTINGS, Constant::KEY_PATH);
    programName = ini.getValue(Constant::SETTINGS, Constant::KEY_PROGRAM_NAME);
    secondsStr = ini.getValue(Constant::SETTINGS, Constant::KEY_SECONDS);

    // 替换环境变量
    path = FileUtil::fixPath(EnvUtil::replaceEnvVars(path));

    // 检查配置值是否为空白字符
    if (StrUtil::isBlank(path)) {
        LogUtil::error(L"error: path 配置值为空白字符或不存在");
        return;
    }
    if (StrUtil::isBlank(programName)) {
        LogUtil::error(L"error: programName 配置值为空白字符或不存在");
        return;
    }

    LogUtil::info(L"配置文件读取成功");


    if (!FileUtil::exists(path)) {
        LogUtil::error(L"error: " + path + L" 主进程路径文件不存在");
        return;
    }


    foo(path, programName, Convert::toInt(secondsStr, 15));


}

[[noreturn]] void foo(const wstring &path, const wstring &programName, int seconds) {
    int count = 1;
    while (true) {

        if (isProcessRunningByName(programName)) {
            // 不需要杀进程，但是程序已启动，则不做任何事
            LogUtil::info(programName + L" 已存启动。" + Convert::toStr(count));
        } else {
            LogUtil::info(programName + L"进程未启动，现在马上启动。" + Convert::toStr(count));

            // 程序未启动，则执行程序
            if (executeFile(path)) {
                LogUtil::info(programName + L" 执行成功");
            } else {
                LogUtil::error(programName + L" 执行失败");
            }
        }

        // 休眠指定的秒数
        this_thread::sleep_for(chrono::seconds(seconds));

        count++;
    }
}


bool executeFile(const std::wstring &programPath) {

    // 使用 ShellExecuteW 替代 ShellExecuteA
    HINSTANCE result = ShellExecuteW(NULL, L"open", programPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    if ((intptr_t) result > 32) {
        return true;
    } else {
        // 添加错误日志
        DWORD error = GetLastError();
        LogUtil::error(L"ShellExecute 失败，错误码: " + std::to_wstring(error));
        return false;
    }

}
