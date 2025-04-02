#include <string>
#include <windows.h>
#include <thread>
#include <atomic>
#include <csignal>

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
#include "TrayIcon.h"

// 全局变量，用于控制程序运行
std::atomic<bool> g_running(true);

bool executeFile(const std::wstring &programPath);

void run();

// 信号处理函数
void signalHandler(int signal) {
    LogUtil::info(L"接收到信号: " + std::to_wstring(signal) + L"，准备退出程序");
    g_running = false;
}


// 保活线程函数
void keepAliveThread() {

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
    int count = 1;
    while (g_running) {

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
        this_thread::sleep_for(chrono::seconds(Convert::toInt(secondsStr, 15)));

        count++;
    }
}




int main(int argc, char *argv[]) {
    // 系统级编码设置
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // C++ 层面编码设置
    std::locale::global(std::locale("en_US.UTF-8"));
    std::wcout.imbue(std::locale());
    std::cout.imbue(std::locale());


    LogUtil::info(L"================ 程序启动 ================");

    // 使用RAII模式确保在函数退出时清理资源
    class LogCleanup {
    public:
        ~LogCleanup() { LogUtil::cleanup(); }
    } logCleanup;


    run();

    LogUtil::info(L"================ 程序结束 ================\n");

    return 0;
}

void run() {


    // 设置信号处理
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // 创建托盘图标
    TrayIcon trayIcon;

    // 设置托盘图标的退出回调
    trayIcon.SetExitCallback([]() {
        LogUtil::info(L"用户通过托盘菜单请求退出");
        g_running = false;
    });

    // 启动托盘图标
    if (!trayIcon.Start()) {
        LogUtil::error(L"启动托盘图标失败");
    }

    // 创建保活线程
    std::thread keepAliveThreadObj(keepAliveThread);

    // 主线程等待，直到收到退出信号
    LogUtil::info(L"主线程等待退出信号");
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 等待保活线程结束
    LogUtil::info(L"等待保活线程结束");
    if (keepAliveThreadObj.joinable()) {
        keepAliveThreadObj.join();
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
