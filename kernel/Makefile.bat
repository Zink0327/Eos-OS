::Eos Makefile

::   Copyright (C) 2023 by Zink
::   This file is part of Eos
::
::    Eos is free software: you can redistribute it and/or modify
::    it under the terms of the GNU General Public License as published by
::    the Free Software Foundation, either version 3 of the License, or
::    (at your option) any later version
::
::    Eos is distributed in the hope that it will be useful,
::    but WITHOUT ANY WARRANTY; without even the implied warranty of
::    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
::    GNU General Public License for more details.

::    You should have received a copy of the GNU General Public License
::    along with this program.  If not, see <https://www.gnu.org/licenses/>.

::src/kernel/Makefile.bat
@echo off
echo entering directory %~dp0
if "%1" equ "clean" (goto clean) else (goto build)

:build

if exist %~dp0..\tools\gcc\bin\x86_64-pc-linux-gcc.exe (
echo %~dp0..\tools\gcc\bin\x86_64-pc-linux-gcc.exe -mcmodel=large -fno-builtin -m64 -c %~dp0print.c -o %~dp0..\build\print.o -fno-stack-protector
%~dp0..\tools\gcc\bin\x86_64-pc-linux-gcc.exe -mcmodel=large -fno-builtin -m64 -c %~dp0print.c -o %~dp0..\build\print.o -fno-stack-protector

if not exist %~dp0..\build\print.o (
set ERRFLAG="1"
echo compile error in print.c!
goto end
)
echo %~dp0..\tools\gcc\bin\x86_64-pc-linux-gcc.exe -mcmodel=large -fno-builtin -m64 -c %~dp0main.c -o %~dp0..\build\main.o -fno-stack-protector
%~dp0..\tools\gcc\bin\x86_64-pc-linux-gcc.exe -mcmodel=large -fno-builtin -m64  -c %~dp0main.c -o %~dp0..\build\main.o -fno-stack-protector

if not exist %~dp0..\build\main.o (
set ERRFLAG="1"
echo compile errorin main.c!
goto end
)



) else (
set ERRFLAG="1"
echo FATAL ERROR: ..\tools\gcc\bin\x86_64-pc-linux-gcc.exe not found!
goto end
)

call %~dp0\init\x86_64\BIOS\Makefile.bat

if %ERRFLAG% equ "1" (
echo ***********An error occured***********
echo       please check the info above
echo **************************************
goto end
)

if exist %~dp0..\tools\gcc\bin\x86_64-pc-linux-ld.exe (
echo %~dp0..\tools\gcc\bin\x86_64-pc-linux-ld.exe -b elf64-x86-64 -z muldefs -o %~dp0..\build\kernel.elf %~dp0..\build\head.o %~dp0..\build\main.o %~dp0..\build\print.o %~dp0..\build\init.o  -T %~dp0linkkernel.lds 
%~dp0..\tools\gcc\bin\x86_64-pc-linux-ld.exe -b elf64-x86-64 -z muldefs -o %~dp0..\build\kernel.elf %~dp0..\build\head.o %~dp0..\build\main.o %~dp0..\build\print.o  %~dp0..\build\init.o -T %~dp0linkkernel.lds 

if not exist %~dp0..\build\kernel.elf (
set ERRFLAG="1"
echo compile error in linking!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: ..\tools\gcc\bin\x86_64-pc-linux-ld.exe not found!
goto end
)


if exist %~dp0..\tools\gcc\bin\x86_64-pc-linux-objcopy.exe (
echo %~dp0..\tools\gcc\bin\x86_64-pc-linux-objcopy.exe -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary %~dp0..\build\kernel.elf %~dp0..\build\kernel.bin
%~dp0..\tools\gcc\bin\x86_64-pc-linux-objcopy.exe -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary %~dp0..\build\kernel.elf %~dp0..\build\kernel.bin

if not exist %~dp0..\build\kernel.bin (
set ERRFLAG="1"
echo compile error in copying!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: ..\tools\gcc\bin\x86_64-pc-linux-objcopy.exe not found!
goto end
)



:end
echo leaving directory %~dp0