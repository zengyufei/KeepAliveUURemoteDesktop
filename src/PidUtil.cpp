#include "PidUtil.h"
#include <windows.h>
#include <tlhelp32.h>
#include "LogUtil.h" // 引入 LogUtil 头文件
#include "StrUtil.h" // 引入 StrUtil 头文件

// Windows 句柄 RAII 包装类
class HandleGuard {
private:
    HANDLE handle;

public:
    HandleGuard(HANDLE h) : handle(h) {}

    ~HandleGuard() {
        if (handle != NULL && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }

    HANDLE get() const { return handle; }

    // 禁止复制
    HandleGuard(const HandleGuard&) = delete;
    HandleGuard& operator=(const HandleGuard&) = delete;
};

// 新增辅助函数
bool findProcessByName(const std::wstring& processName, DWORD& processID) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    // 使用 RAII 管理句柄
    HandleGuard snapshotGuard(hSnapshot);

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(snapshotGuard.get(), &pe)) {
        return false;
    }

    do {
        if (std::wstring(pe.szExeFile) == processName) {
            processID = pe.th32ProcessID;
            return true;
        }
    } while (Process32NextW(snapshotGuard.get(), &pe));

    return false;
}

bool terminateProcessByName(const std::wstring& processName) {
    DWORD processID;
    if (!findProcessByName(processName, processID)) {
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
    if (hProcess == NULL) {
        return false;
    }

    // 使用 RAII 管理进程句柄
    HandleGuard processGuard(hProcess);

    return TerminateProcess(processGuard.get(), 0);
}

bool isProcessRunningByName(const std::wstring& processName) {
    DWORD processID;
    return findProcessByName(processName, processID);
}