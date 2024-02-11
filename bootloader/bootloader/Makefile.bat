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

::./bootloader/Makefile.bat

@echo off
echo entering directory %cd%
goto build

:build
if "%1" equ "floppy" (
call %~dp0\floppy\Makefile.bat
) else (
if "%1" equ "harddisk" (
call %~dp0\harddisk\Makefile.bat
) else (
echo invalid input
))
goto end

:end
echo leaving directory %~dp0