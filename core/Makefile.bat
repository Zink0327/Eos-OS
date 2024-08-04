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

::src/core/Makefile.bat
::@echo off
echo entering directory %~dp0
if "%1" equ "clean" (goto clean) else (goto build)

:build
copy %~dp0init\%ARCH%\%TYPE%\%SUBTYPE%\* %~dp0
copy %~dp0init\%ARCH%\* %~dp0

if exist %WORKROOT%tools\gcc\bin\%CC% (
dir %~dp0*.c /b > cfile.txt
::although the file names of output files might be a little bit ugly, but I've tried my best
for /f %%m in (cfile.txt) do (
echo %WORKROOT%tools\gcc\bin\%CC% -Wall -mcmodel=large -fno-builtin -m64 -c %~dp0%%m -o %WORKROOT%build\%%m.o -fno-stack-protector
%WORKROOT%tools\gcc\bin\%CC% -Wall -mcmodel=large -fno-builtin -m64 -c %~dp0%%m -o %WORKROOT%build\%%m.o -fno-stack-protector
if not exist %WORKROOT%build\%%m.o (
set ERRFLAG="1"
echo error in compiling %%m!
goto end
)
)
del cfile.txt /q
dir %~dp0*.S /b > sfile.txt
for /f %%m in (sfile.txt) do (
echo %WORKROOT%tools\gcc\bin\%CC% -E  %~dp0%%m > %WORKROOT%build\%%m.s
%WORKROOT%tools\gcc\bin\%CC% -E  %~dp0%%m > %WORKROOT%build\%%m.s
if not exist %WORKROOT%build\%%m.s (
set ERRFLAG="1"
echo error in compiling %%m!
goto end
)
)
del sfile.txt /q
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%CC% not found!
goto end
)

if exist %WORKROOT%tools\nasm\nasm.exe (
dir %~dp0*.asm /b > asfile.txt
for /f %%m in (asfile.txt) do (
echo %WORKROOT%tools\nasm\nasm.exe -f elf64 %~dp0%%m -o %WORKROOT%build\%%m.o
%WORKROOT%tools\nasm\nasm.exe -f elf64 %~dp0%%m -o %WORKROOT%build\%%m.o
if not exist %WORKROOT%build\%%m.o (
set ERRFLAG="1"
echo error in compiling %%m!
goto end
)
)
del asfile.txt /q
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\nasm\nasm.exe not found!
goto end
)

if exist %WORKROOT%tools\gcc\bin\%AS% (
dir %WORKROOT%build\*.s /b > ssfile.txt
for /f %%m in (ssfile.txt) do (
echo %WORKROOT%tools\gcc\bin\%AS% --64 %WORKROOT%build\%%m -o %WORKROOT%build\%%m.o
%WORKROOT%tools\gcc\bin\%AS% --64 %WORKROOT%build\%%m -o %WORKROOT%build\%%m.o
if not exist %WORKROOT%build\%%m.o (
set ERRFLAG="1"
echo error in compiling %%m!
goto end
)
)
del ssfile.txt /q
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%AS% not found!
goto end
)


if exist %WORKROOT%tools\gcc\bin\%LD% (
echo %WORKROOT%tools\gcc\bin\%LD% -b %LDLINKFMT% -z muldefs -o %WORKROOT%build\kernel.elf  %OBJS% -T %~dp0linkkernel.lds 
%WORKROOT%tools\gcc\bin\%LD% -b %LDLINKFMT% -z muldefs -o %WORKROOT%build\kernel.elf %OBJS% -T %~dp0linkkernel.lds 

if not exist %WORKROOT%build\kernel.elf (
set ERRFLAG="1"
echo error in linking!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%LD% not found!
goto end
)


if exist %WORKROOT%tools\gcc\bin\%OBJCOPY% (
echo %WORKROOT%tools\gcc\bin\%OBJCOPY% -I %LDLINKFMT% -S -R ".eh_frame" -R ".comment" -O binary %WORKROOT%build\kernel.elf %WORKROOT%build\carbon.bin
%WORKROOT%tools\gcc\bin\%OBJCOPY% -I %LDLINKFMT% -S -R ".eh_frame" -R ".comment" -O binary %WORKROOT%build\kernel.elf %WORKROOT%build\carbon.bin

if not exist %WORKROOT%build\carbon.bin (
set ERRFLAG="1"
echo error in generating carbon.bin!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%OBJCOPY% not found!
goto end
)

:clean 
dir %~dp0init\%ARCH%\%TYPE%\%SUBTYPE%\ /b > initfl.txt
dir %~dp0init\%ARCH%\ /b >> initfl.txt

for /f %%i in (initfl.txt) do del %~dp0%%i /q

del initfl.txt /q
goto end

:end
echo leaving directory %~dp0