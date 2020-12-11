#pragma once

typedef BOOL (*ACTION)(LPCSTR fileName, LPVOID arg);

BOOL TraverseDirTree(LPCSTR dir, LPCSTR extension, ACTION  action, LPVOID arg);
