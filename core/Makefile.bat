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
@echo off
echo entering directory %~dp0
if "%1" equ "clean" (goto clean) else (goto build)

:build

copy %~dp0init\%ARCH%\%BOOTWAY%\* %~dp0

if exist %WORKROOT%tools\gcc\bin\%CC% (
echo %WORKROOT%tools\gcc\bin\%CC% -mcmodel=large -fno-builtin -m64 -c %~dp0print.c -o %WORKROOT%build\print.o -fno-stack-protector
%WORKROOT%tools\gcc\bin\%CC% -mcmodel=large -fno-builtin -m64 -c %~dp0print.c -o %WORKROOT%build\print.o -fno-stack-protector

if not exist %WORKROOT%build\print.o (
set ERRFLAG="1"
echo error in compiling print.c!
goto end
)
echo %WORKROOT%tools\gcc\bin\%CC% -mcmodel=large -fno-builtin -m64 -c %~dp0main.c -o %WORKROOT%build\main.o -fno-stack-protector
%WORKROOT%tools\gcc\bin\%CC% -mcmodel=large -fno-builtin -m64  -c %~dp0main.c -o %WORKROOT%build\main.o -fno-stack-protector

if not exist %WORKROOT%build\main.o (
set ERRFLAG="1"
echo error in compiling main.c!
goto end
)

echo %WORKROOT%tools\gcc\bin\%CC% -mcmodel=large -fno-builtin -m64 -c %~dp0init.c -o %WORKROOT%build\init.o -fno-stack-protector
%WORKROOT%tools\gcc\bin\%CC% -mcmodel=large -fno-builtin -m64 -c %~dp0init.c -o %WORKROOT%build\init.o -fno-stack-protector

if not exist %WORKROOT%build\init.o (
set ERRFLAG="1"
echo error in compiling init.c!
goto end
)

echo %WORKROOT%tools\gcc\bin\%CC% -E  %~dp0head.S > %WORKROOT%build\head.s
%WORKROOT%tools\gcc\bin\%CC% -E  %~dp0head.S > %WORKROOT%build\head.s

if not exist %WORKROOT%build\head.s (
set ERRFLAG="1"
echo error in generating head.S!
goto end
)


) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%CC% not found!
goto end
)

if exist %WORKROOT%tools\gcc\bin\%AS% (
echo %WORKROOT%tools\gcc\bin\%AS% --64 %WORKROOT%build\head.s -o %WORKROOT%build\head.o
%WORKROOT%tools\gcc\bin\%AS% --64 %WORKROOT%build\head.s -o %WORKROOT%build\head.o

if not exist %WORKROOT%build\head.o (
set ERRFLAG="1"
echo error in compiling head.s!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%AS% not found!
goto end
)


if exist %WORKROOT%tools\gcc\bin\%LD% (
echo %WORKROOT%tools\gcc\bin\%LD% -b %LDLINKFMT% -z muldefs -o %WORKROOT%build\kernel.elf %WORKROOT%build\head.o %WORKROOT%build\main.o %WORKROOT%build\print.o %WORKROOT%build\init.o  -T %~dp0linkkernel.lds 
%WORKROOT%tools\gcc\bin\%LD% -b %LDLINKFMT% -z muldefs -o %WORKROOT%build\kernel.elf %WORKROOT%build\head.o %WORKROOT%build\main.o %WORKROOT%build\print.o  %WORKROOT%build\init.o -T %~dp0linkkernel.lds 

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
echo compile error in generating carbon.bin!
goto end
)
) else (
set ERRFLAG="1"
echo FATAL ERROR: %WORKROOT%tools\gcc\bin\%OBJCOPY% not found!
goto end
)



:end
echo leaving directory %~dp0