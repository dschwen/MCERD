README -file for MCERD (compiling instructions)

See manual.pdf for more information about the program.

============================================


------------------------------------------
Contents of this file

1. Compiling
  - Windows
  - Linux
2. Compiling using makefiles
  - Windows
  - Linux
3. Problems with GtkExtra library
------------------------------------------



Note: You should have GTK+ 2.x installed on your system before compiling MCERD.


1. Compiling
==========


Windows:

Extract MCERD packet, and in the MCERD directory..

- Run winmake.bat to compile mcerd.exe

- Run mcerd.exe to run the program

--------------------------------------------------------------------------------

Linux:

Extract MCERD packet, and in the MCERD directory..

- Type `./linuxmake' to compile mcerd binary

- Type `./mcerd' to run the program

Note: If you have permission problem with linuxmake script, type `chmod 777 linuxmake'

--------------------------------------------------------------------------------



2. Compiling using makefiles
======================

If problems occurs when compiling, you can try to compile 'manually' using makefiles.


Windows:

- Extract MCERD packet for example to C:\MCERD

- In the directory C:\MCERD\src\zbl96-0.99a run 'winmake_zbl' to get zbl96 binary and libzbl96.a library

- Copy zbl96 binary and libzbl96.a from C:\MCERD\src\zbl96-0.99a to C:\MCERD\src

- In the directory C:\MCERD\src run 'winmake_mcerd' to get mcerd binary

- Copy mcerd binary from C:\MCERD\src to C:\MCERD

- In the directory C:\MCERD run the program with command 'mcerd'

For generating SRIM stopping powers:

- Copy generate_sptables binary from C:\MCERD\src to C:\MCERD

- In the directory C:\MCERD run the program with command 'generate_sptables'

--------------------------------------------------------------------------------

Linux:

- Extract MCERD packet for example to /home/MCERD

- In the directory /home/MCERD/src/zbl96-0.99a run 'make' to get zbl96 binary and run 'make lib' to get libzbl96.a library

- Copy zbl96 and libzbl96.a from /home/MCERD/src/zbl96-0.99a to /home/MCERD/src

- In the directory /home/MCERD/src run 'make' to get mcerd binary

- Copy mcerd binary from /home/MCERD/src to /home/MCERD

- In the directory /home/MCERD run the program with command './mcerd'

--------------------------------------------------------------------------------



3. Problems with GtkExtra library
=========================

If you get error message about GtkExtra library when compiling, GtkExtra library in MCERD packet may not work on your architecture. MCERD packet includes GtkExtra libraries for Windows 32-bit and Linux 64-bit. You can compile GtkExtra library in your architecture:

- Download GtkExtra (version 2.1.1) from http://gtkextra.sourceforge.net/

- Extract the packet and compile the library (installation instructions should come with the packet)

- Copy the compiled library (libgtkextra-x11-2.0.a) to MCERD src folder

- Compile MCERD

Note: GtkExtra library name used in MCERD is specified in makefile (in MCERD src folder), Makefile for Linux and winmake_mcerd.bat for Windows. Change the library name to correspond with the used library.

--------------------------------------------------------------------------------
