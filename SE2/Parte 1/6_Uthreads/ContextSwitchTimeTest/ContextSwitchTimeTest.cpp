#include "stdafx.h"
#include "..\Include\Uthread.h"

#define MAX_ITERATIONS 1<<20 // 1MB
#define STACK_SIZE (16 * 4096)

HANDLE ThreadA;
HANDLE ThreadB;

VOID Test_Func(UT_ARGUMENT Argument) {
	for (int i = 0; i < MAX_ITERATIONS; i++)
		UtYield();
}


VOID Test() {
	ThreadA = UtCreate(Test_Func, (UT_ARGUMENT) 'A', STACK_SIZE, "Thread A");
	ThreadB = UtCreate(Test_Func, (UT_ARGUMENT) 'B', STACK_SIZE, "Thread B");
	long double start = GetTickCount();

	UtRun();

	long double end = GetTickCount();
	long double time = (end - start) / ((MAX_ITERATIONS) * 2);
	printf("Each commutation is approximately %lf miliseconds.\n", time);
}


int main() {
	UtInit();

	Test();

	printf("Press any key to finish");
	getchar();

	UtEnd();

	return 0;
}
