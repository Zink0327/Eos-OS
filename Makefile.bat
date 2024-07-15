::EOS Makefile
::Copyright (C) by Zink


::@echo off
echo Build started...

set ERRFLAG="0"

if "%1" equ "clean" ( 
goto clean 
) else ( 
goto build 
)

:build

call .\Makefile clean

call .\bootloader\Makefile floppy floppy

if %ERRFLAG% equ "1" (
echo ***********An error occured***********
echo       please check the info above
echo **************************************
goto end
)

call .\core\Makefile.bat

if %ERRFLAG% equ "1" (
echo ***********An error occured***********
echo       please check the info above
echo **************************************
goto end
)


echo *******Builded succeesfully!********

goto end

:clean
call %~dp0\core\Makefile.bat clean
goto end

:end

