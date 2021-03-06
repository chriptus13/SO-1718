#include "stdafx.h"
#include "..\Include\Uthread.h"

#define STACK_SIZE (16 * 4096)

HANDLE ThreadA;
HANDLE ThreadB;
HANDLE ThreadC;

VOID Test_Func_A(UT_ARGUMENT Argument) {
	UCHAR Char = (UCHAR)Argument;
	printf("\n :: Thread %c - BEGIN :: \n\n", Char);
	
	UtDump();
	
	HANDLE handle[] = { ThreadB, ThreadC };
	UtMultJoin(handle, 2);
	
	printf("\n :: Thread %c - ALMOST ENDING :: \n\n", Char);
	
	UtDump();
	
	printf("\n :: Thread %c - END :: \n\n", Char);
}

VOID Test_Func_B(UT_ARGUMENT Argument) {
	UCHAR Char = (UCHAR)Argument;
	
	printf("\n :: Thread %c - BEGIN :: \n\n", Char);
	
	UtDump();
	
	printf("\n :: Thread %c - ALMOST ENDING :: \n\n", Char);
	
	UtDump();
	
	printf("\n :: Thread %c - END :: \n\n", Char);
}

VOID Test_Func_C(UT_ARGUMENT Argument) {
	UCHAR Char = (UCHAR)Argument;
	
	printf("\n :: Thread %c - BEGIN :: \n\n", Char);
	
	UtDump();
	
	printf("\n :: Thread %c - END :: \n\n", Char);
}


VOID Test() {
	ThreadA = UtCreate(Test_Func_A, (UT_ARGUMENT) 'A', STACK_SIZE, "Thread A");
	ThreadB = UtCreate(Test_Func_B, (UT_ARGUMENT) 'B', STACK_SIZE, "Thread B");
	ThreadC = UtCreate(Test_Func_C, (UT_ARGUMENT) 'C', STACK_SIZE, "Thread C");
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
