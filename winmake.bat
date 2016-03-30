:: windows bat-file for compiling and copying all mcerd related files. Run winmake to compile.
:: version 17.12.2010
::
@ECHO OFF

:: compiling zbl binary and library
ECHO compiling zbl96
CD %CD%\src\zbl96-0.99a
CALL winmake_zbl.bat
CD ..

:: copying zbl binary and library to src folder
ECHO copying zbl96
COPY %CD%\zbl96-0.99a\zbl96.exe %CD%
COPY %CD%\zbl96-0.99a\libzbl96.a %CD%
DEL %CD%\zbl96-0.99a\zbl96.exe
DEL %CD%\zbl96-0.99a\libzbl96.a

:: compiling mcerd binary
ECHO compiling mcerd
CALL winmake_mcerd.bat
CD ..

:: copying mcerd and generate_sptables binaries to main folder
ECHO copying mcerd
COPY %CD%\src\mcerd.exe %CD%
COPY %CD%\src\generate_sptables.exe %CD%
DEL %CD%\src\mcerd.exe
DEL %CD%\src\generate_sptables.exe
DEL %CD%\src\zbl96.exe
DEL %CD%\src\libzbl96.a
ECHO.
ECHO Finished.
