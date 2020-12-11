CFLAGS= /Zi
ALL= ContextSwitchTime.exe ProcessAux.exe
all: $(ALL)

ProcessAux: ProcessAux.cpp
	cl /Zi ProcessAux.cpp

ContextSwitchTime.exe: ContextSwitchTime.cpp ProcessAux.exe
	cl /Zi ContextSwitchTime.cpp

clean:
	del /Q $(ALL) *.exe *.obj *.asm *.dll *.rar *.ilk *.pdb *.suo
