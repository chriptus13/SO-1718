#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <process.h>

#define MAX_ITERATIONS 1<<20 // 1MB

int main() {
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		SwitchToThread();
	}
	return 0;
}