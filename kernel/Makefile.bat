::Omega OS Makefile
::Copyright (C) 2023 by Zink
::This file is part of Omega OS

::Omega OS  is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. 

::This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

::You should have received a copy of the GNU General Public License along with Omega OS. If not, see <https://www.gnu.org/licenses/>.
::src/kernel/Makefile.bat

::@echo off
echo entering directory %~dp0
if "%1" equ "clean" (goto clean) else (goto build)
set objs=head.o main.o 
:clean
del *.bin
del *.lst
goto end

:build
where /q gcc.exe
if ERRORLEVEL 1 (
echo ERROR:GCC compiler (gcc.exe) not found! Ensure it is installed and placed in your PATH.
goto end
) else (
echo GCC compiler found!
)
where /q as.exe
if ERRORLEVEL 1 (
echo ERROR:GCC GAS (as.exe) compiler not found! Ensure it is installed and placed in your PATH.
goto end
) else (
echo GCC GAS compiler found!
)
where /q ld.exe
if ERRORLEVEL 1 (
echo ERROR:GCC linker (ld.exe) not found! Ensure it is installed and placed in your PATH.
goto end
) else (
echo GCC linker found!
)
where /q objcopy.exe
if ERRORLEVEL 1 (
echo ERROR: (objcopy.exe) not found! Ensure it is installed and placed in your PATH.
goto end
) else (
echo objcopy.exe found!
)


::build *.S
echo gcc -E  head.S > %~dp0..\build\head.s

gcc -E  head.S > %~dp0..\build\head.s

echo as --64 -o %~dp0..\build\head.o  %~dp0..\build\head.s

as --64 -o %~dp0..\build\head.o  %~dp0..\build\head.s

if not exist %~dp0..\build\head.o (
echo compile error!
goto end
)

::build *.c

echo gcc  -mcmodel=large -fno-builtin -m64 -c main.c -o %~dp0..\build\main.o  -fno-stack-protector

gcc  -mcmodel=large -fno-builtin -m64 -c main.c -o %~dp0..\build\main.o  -fno-stack-protector



if not exist %~dp0..\build\main.o (
echo compile error!
goto end
)



echo ld -b elf64-x86-64 -z muldefs -o %~dp0..\build\system %objs% -T linkkernel.lds 

ld -b elf64-x86-64 -z muldefs -o %~dp0..\build\system %objs% -T linkkernel.lds 
if not exist %~dp0..\build\system (
echo link error!
goto end
)
echo objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary %~dp0..\build\system kernel.bin

objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary %~dp0..\build\system kernel.bin

goto end

:end
echo leaving directory %~dp0
