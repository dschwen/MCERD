:: windows bat-file for compiling zbl96. Run winmake_zbl to compile.
:: version 17.12.2010
::
@ECHO OFF
SET srcs=zbl96stop.c zbl96main.c zbl96lib.c
SET binaryobjs=zbl96stop.o zbl96main.o
SET libobjs=zbl96stop.o zbl96lib.o
SET libs=-lm
SET flags=-O2
SET program=zbl96
SET library=libzbl96.a
ECHO.
ECHO compiling zbl binary..
ECHO.
gcc %flags% -c %srcs%
gcc -o %program% %binaryobjs% %libs%
ECHO.
ECHO compiling zbl library..
ar r %library% %libobjs%
ranlib %library%
ECHO.
ECHO removing object files..
ECHO.
DEL *.o
SET srcs=
SET binaryobjs=
SET libobjs=
SET libs=
SET flags=
SET program=
SET library=
ECHO zbl96 ready.
