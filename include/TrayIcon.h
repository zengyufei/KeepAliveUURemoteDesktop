#ifndef KEEPALIVEUUREMOTEDESKTOP_TRAYICON_H
#define KEEPALIVEUUREMOTEDESKTOP_TRAYICON_H

#include <windows.h>
#include <thread>
#include <atomic>
#include <functional>
#include <memory>

class TrayIcon {
private:
    HWND hwnd;
    NOTIFYICONDATA nid;
    std::unique_ptr<std::thread> trayThread;
    std::atomic<bool> running;

    // 窗口过程函数必须是静态的
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 实际的消息处理函数
    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 托盘线程函数
    void TrayThreadFunction();

public:
    TrayIcon();
    ~TrayIcon();

    // 启动托盘图标
    bool Start();

    // 停止托盘图标
    void Stop();

    // 设置回调函数
    void SetExitCallback(std::function<void()> callback);

private:
    std::function<void()> exitCallback;
};

#endif // KEEPALIVEUUREMOTEDESKTOP_TRAYICON_H