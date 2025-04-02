#include "TrayIcon.h"
#include "resource.h"
#include "LogUtil.h"
#include <string>

// 定义窗口类名
#define WM_TRAYICON (WM_USER + 1)
#define WND_CLASS_NAME L"KeepAliveUURemoteDesktopTrayWnd"

TrayIcon::TrayIcon() : hwnd(NULL), running(false) {
    // 初始化
    memset(&nid, 0, sizeof(nid));
}

TrayIcon::~TrayIcon() {
    Stop();
}

bool TrayIcon::Start() {
    if (running) {
        return true; // 已经运行中
    }

    running = true;

    // 使用make_unique创建线程
    trayThread = std::make_unique<std::thread>(&TrayIcon::TrayThreadFunction, this);

    return true;
}

void TrayIcon::Stop() {
    if (!running) {
        return; // 已经停止
    }

    running = false;

    // 发送消息给窗口，触发窗口销毁
    if (hwnd) {
        SendMessage(hwnd, WM_CLOSE, 0, 0);
    }

    // 等待线程结束
    if (trayThread && trayThread->joinable()) {
        trayThread->join();
    }

    // 释放线程资源
    trayThread.reset();

    // 移除托盘图标
    if (hwnd) {
        Shell_NotifyIcon(NIM_DELETE, &nid);
    }
}

void TrayIcon::SetExitCallback(std::function<void()> callback) {
    exitCallback = callback;
}

void TrayIcon::TrayThreadFunction() {
    // 创建一个最小化的隐藏窗口
    hwnd = CreateWindowEx(
            0, L"STATIC", L"TrayIconWindow",
            WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL,
            GetModuleHandle(NULL), NULL
    );

    if (!hwnd) {
        LogUtil::error(L"Failed to create window");
        return;
    }

    // 子类化窗口过程
    WNDPROC oldProc = (WNDPROC)SetWindowLongPtr(
            hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc
    );
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);

    // 初始化托盘图标
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    wcscpy_s(nid.szTip, L"UU远程桌面保活工具");

    // 添加托盘图标
    Shell_NotifyIcon(NIM_ADD, &nid);

    // 消息循环
    MSG msg;
    while (running && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 移除托盘图标
    Shell_NotifyIcon(NIM_DELETE, &nid);

    // 恢复原始窗口过程
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oldProc);

    // 销毁窗口
    DestroyWindow(hwnd);
}

LRESULT CALLBACK TrayIcon::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // 获取this指针
    TrayIcon* pThis = reinterpret_cast<TrayIcon*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (pThis) {
        return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT TrayIcon::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                // 显示右键菜单 - 仅在需要时加载菜单
                POINT pt;
                GetCursorPos(&pt);

                // 仅在用户点击时加载菜单
                HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU));
                if (hMenu) {
                    HMENU hSubMenu = GetSubMenu(hMenu, 0);
                    if (hSubMenu) {
                        // 必须设置前台窗口，否则菜单会立即消失
                        SetForegroundWindow(hwnd);

                        // 显示菜单
                        TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON,
                                       pt.x, pt.y, 0, hwnd, NULL);

                        // 发送空消息，参见MSDN文档
                        PostMessage(hwnd, WM_NULL, 0, 0);
                    }
                    DestroyMenu(hMenu); // 确保菜单被销毁
                }
            }
            return 0;

        case WM_COMMAND:
            // 处理菜单命令
            switch (LOWORD(wParam)) {
                case ID_MENU_EXIT:
                    if (exitCallback) {
                        exitCallback();
                    }
                    DestroyWindow(hwnd);
                    return 0;

                case ID_MENU_ABOUT:
                    MessageBox(hwnd, L"UU远程桌面保活工具\n版本: 1.0", L"关于", MB_OK | MB_ICONINFORMATION);
                    return 0;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}