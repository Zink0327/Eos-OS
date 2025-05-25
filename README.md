# Eos Operating System (EOS)

## Introduction

### What is Eos?

> Eos was the goddess of the dawn. Each day, she rose from the Ocean and rode her chariot across the sky, dispersing the shadows of the night. (quoted from [https://mythopedia.com/topics/eos](https://mythopedia.com/topics/eos))

### What is EOS?

EOS (all capital letters) stands for Eos Operating System. You can either read this like "E-O-S", or just pronounce it as Eos.

EOS is an operating system that is free software—that means, it respects users' freedom. EOS contains some useful software (which is also free software), either written by us, or released by third parties.

### License

EOS is released under the terms of GNU General Public License version 3. See COPYING for more details.

## Compiling

EOS is designed to be compiled under Windows (but I'll appreciate a lot if you write a Makefile script for the compiling). 

To avoid bringing something not nice to the source code, you have to create a folder to put the source code folder into it and rename the source code folder as src. You have to create a folder by the source code folder called "tools", then put [Netwide Assembler (NASM)](https://nasm.us/) into tools/nasm, 

GCC into tools/gcc ( **GCC must be able to create ELF file under Windows.** You can find an introduction about how to compile one [here: https://wiki.osdev.org/GCC_Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler)) (You can use the following configuration before compiling gcc: ../configure --prefix=/home/opt --target=x86_64-pc-linux --build=x86_64-pc-linux --host=x86_64-w64-mingw32 --disable-nls --enable-languages=c,c++ --without-headers) 

(Theoretically you can just use mingw32-w64, but I haven't test that yet)

and edimg.exe into tools/edimg (you can find it in tools/edimg in this repository). 

Then, paste the following content into ...

tools/ccrule.bat:

```
if exist %WORKROOT%tools\gcc\bin\%CC% (

echo    CC %2
%WORKROOT%tools\gcc\bin\%CC% %cc_param% -c %1%2 -o %WORKROOT%build\%2.o
if not exist %WORKROOT%build\%2.o (
set ERRFLAG="1"
echo error in compiling %2!
goto end
)

) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%CC% not found!
goto end
)
:end
```
tools/asrule.bat:

```
if exist %WORKROOT%tools\gcc\bin\%AS% (
echo    AS %1
%WORKROOT%tools\gcc\bin\%AS% %as_param% %WORKROOT%build\%1 -o %WORKROOT%build\%1.o
if not exist %WORKROOT%build\%1.o (
set ERRFLAG="1"
echo error in compiling %1!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%AS% not found!
goto end
)
```


After that, the folder will look like this:

Eos-Build-Env

├─tools

│  ├─edimg

│  ├─nasm

│  ├─gcc

│  ├─asrule.bat

│  └─ccrule.bat

├─build

├─src

└─(make.bat)

you have to create a file "make.bat" (or any other name you like), and fill this file with the following content:

```
@echo off
:: This file is the script of the compilation of the EOS. Run this first.

::------------CONFIGURE------------
::Feel free to edit the following configuration.

:: configure the following compiler if you want to cross compile
:: MAKE SURE THAT THE EXECUTABLE FILES HAVE BEEN PUT IN tools/gcc/bin
set CC=x86_64-pc-linux-gcc.exe
set AS=x86_64-pc-linux-as.exe
set LD=x86_64-pc-linux-ld.exe
set OBJCOPY=x86_64-pc-linux-objcopy.exe
set LDLINKFMT=elf64-x86-64
set NASM=nasm

set cc_param=-Wall -mcmodel=large -fno-builtin -m64 -fno-stack-protector
set as_param=--64
:: configure the arch and boot way
set ARCH=x86_64
set TYPE=BIOS
::if your architecture don't have specific type, leave it empty
set SUBTYPE=

::--------END OF CONFIGURE----------

set WORKROOT=%~dp0

::set your objs here...
set OBJS=%WORKROOT%build\head.S.s.o %WORKROOT%build\main.c.o %WORKROOT%build\print.c.o  %WORKROOT%build\init.c.o %WORKROOT%build\int.c.o %WORKROOT%build\int.asm.o %WORKROOT%build\memory.c.o %WORKROOT%build\asmfun.asm.o %WORKROOT%build\memblk.c.o

mkdir build

if "%1" equ "clean" ( 
goto clean
) else ( 
del EOS.img
cd src
call Makefile.bat
cd ..
)

if exist .\tools\edimg\edimg.exe (

echo %WORKROOT%\tools\edimg\edimg.exe imgin:./tools/edimg/fdimg0at.tek wbinimg src:%WORKROOT%\build\mbr.bin len:512 from:0 to:0 copy from:%WORKROOT%\build\loader.bin to:@: copy from:%WORKROOT%\build\carbon.bin to:@: imgout:%WORKROOT%EOS.img

%WORKROOT%\tools\edimg\edimg.exe imgin:./tools/edimg/fdimg0at.tek wbinimg src:%WORKROOT%\build\mbr.bin len:512 from:0 to:0 copy from:%WORKROOT%\build\loader.bin to:@: copy from:%WORKROOT%\build\carbon.bin to:@: imgout:%WORKROOT%EOS.img

) else (
echo ERROR:.\tools\edimg\edimg.exe not found!
)
goto end

:clean
del .\build /q
call .\src\Makefile.bat clean
:end
pause
```

just double click the make.bat in the root directory and it will do everything for you. ;-) 

If you want to do cross-compile, you have to edit 2 file: make.bat above and global.h at path/to/Eos-Build-Env/src/core/ to set the architecture and compiler.
