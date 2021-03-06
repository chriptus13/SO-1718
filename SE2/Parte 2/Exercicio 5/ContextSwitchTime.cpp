#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <process.h>

#define MAX_ITERATIONS 1<<20 // 1MB
HANDLE threadA;
HANDLE threadB;
unsigned WINAPI DoWork(PVOID);

VOID TestContextSwitchInSameProcess() {
	threadA = (HANDLE)_beginthreadex(NULL, 0, &DoWork, NULL, CREATE_SUSPENDED, NULL);
	threadB = (HANDLE)_beginthreadex(NULL, 0, &DoWork, NULL, CREATE_SUSPENDED, NULL);
	SetThreadPriority(threadB, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(threadB, 1);

	SetThreadPriority(threadA, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(threadA, 1);
	long double start = GetTickCount();
	ResumeThread(threadA);
	ResumeThread(threadB);
	WaitForSingleObject(threadA, INFINITE);
	WaitForSingleObject(threadB, INFINITE);
	long double end = GetTickCount();
	long double time = end - start;
	printf("Each commutation in same process is approximately %lf miliseconds.\n", time / (MAX_ITERATIONS) * 2);
	printf("Press any key to continue!");
	getchar();
}

VOID TestContextSwitchInDifferentProcess() {
	threadA = (HANDLE)_beginthreadex(NULL, 0, &DoWork, NULL, CREATE_SUSPENDED, NULL);
	
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, _T("ProcessAux"), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)){
		printf("Error\n");
	}
	threadB = pi.hThread;
	
	SetThreadPriority(threadB, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(threadB, 1);

	SetThreadPriority(threadA, THREAD_PRIORITY_HIGHEST);
	SetThreadAffinityMask(threadA, 1);
	long double start = GetTickCount();
	ResumeThread(threadA);
	ResumeThread(threadB);
	WaitForSingleObject(threadA, INFINITE);
	WaitForSingleObject(threadB, INFINITE);
	
	long double end = GetTickCount();
	long double time = end - start;
	printf("Each commutation in different processes is approximately %lf miliseconds.\n", time / (MAX_ITERATIONS) * 2);
	printf("Press any key to continue!");
	getchar();
}

int main() {
	TestContextSwitchInSameProcess();
	TestContextSwitchInDifferentProcess();
	return 0;
}

unsigned WINAPI DoWork(PVOID args) {
	for (int i = 0; i < MAX_ITERATIONS; i++) {
		SwitchToThread();
	}
	return 0;
}

