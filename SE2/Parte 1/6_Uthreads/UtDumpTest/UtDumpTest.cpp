#include "stdafx.h"
#include "..\Include\Uthread.h"

#define STACK_SIZE (16 * 4096)

VOID Test_Func(UT_ARGUMENT Argument) {
	UCHAR Char = (UCHAR)Argument;
	printf("\n :: Thread %c - BEGIN :: \n\n", Char);
	UtDump();
	UtYield();
	printf("\n :: Thread %c - ALMOST ENDING :: \n\n", Char);
	UtDump();
	printf("\n :: Thread %c - END :: \n\n", Char);
}

VOID Test() {
	HANDLE ThreadA = UtCreate(Test_Func, (UT_ARGUMENT) 'A', STACK_SIZE, "Thread A");
	HANDLE ThreadB = UtCreate(Test_Func, (UT_ARGUMENT) 'B', STACK_SIZE / 2, "Thread B");
	HANDLE ThreadC = UtCreate(Test_Func, (UT_ARGUMENT) 'C', STACK_SIZE / 4, "Thread C");
	UtRun();
}


int main() {
	UtInit();

	Test();

	printf("Press any key to finish");
	getchar();

	UtEnd();

	return 0;
}
