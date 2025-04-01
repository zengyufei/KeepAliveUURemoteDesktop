#ifndef PIDUTIL_H
#define PIDUTIL_H

#include <string>

bool terminateProcessByName(const std::wstring& processName);
bool isProcessRunningByName(const std::wstring& processName);

#endif // PIDUTIL_H