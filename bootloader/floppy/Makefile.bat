::EOS Makefile

::   Copyright (C) 2023 by Zink
::   This file is part of EOS
::
::    EOS is free software: you can redistribute it and/or modify
::    it under the terms of the GNU General Public License as published by
::    the Free Software Foundation, either version 3 of the License, or
::    (at your option) any later version
::
::    EOS is distributed in the hope that it will be useful,
::    but WITHOUT ANY WARRANTY; without even the implied warranty of
::    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
::    GNU General Public License for more details.

::    You should have received a copy of the GNU General Public License
::    along with this program.  If not, see <https://www.gnu.org/licenses/>.

::src/bootloader/floppy/Makefile.bat

::@echo off
echo entering directory %~dp0
if "%1" equ "clean" (goto clean) else (goto build)

:clean
del *.bin
del *.lst
goto end

:build
if exist %WORKROOT%tools\nasm\nasm.exe (
%WORKROOT%tools\nasm\nasm.exe %~dp0\mbr.asm -o %WORKROOT%build\mbr.bin -l %WORKROOT%build\mbr.lst
echo %WORKROOT%tools\nasm\nasm.exe %~dp0\mbr.asm -o %WORKROOT%build\mbr.bin -l %WORKROOT%build\mbr.lst
if not exist %WORKROOT%build\mbr.bin (
set ERRFLAG="1"
echo compile error!
goto end
)
echo %WORKROOT%tools\nasm\nasm.exe %~dp0\loader.asm -i %~dp0 -o %WORKROOT%build\loader.bin -l %WORKROOT%build\loader.lst
%WORKROOT%tools\nasm\nasm.exe %~dp0\loader.asm -i %~dp0 -o %WORKROOT%build\loader.bin -l %WORKROOT%build\loader.lst
if not exist %WORKROOT%build\loader.bin (
set ERRFLAG="1"
echo compile error!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\nasm\nasm.exe not found!
)
goto end

:end
echo leaving directory %~dp0