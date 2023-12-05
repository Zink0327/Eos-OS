::Eos Makefile
::Copyright (C) by Zink


::@echo off
echo Build started...
if "%1" equ "clean" ( 
goto clean 
) else if "%1" equ "prebuilt" ( 
goto prebuilt 
) else ( 
goto build 
)

:build
mkdir build
call .\bootloader\Makefile floppy floppy

::call .\kernel\Makefile.bat
::copy .\kernel\kernel.bin kernel.bin

if exist .\tools\edimg\edimg.exe (

echo .\tools\edimg\edimg.exe imgin:./tools/edimg/fdimg0at.tek wbinimg src:.\build\mbr.bin len:512 from:0 to:0 copy from:.\build\loader.bin to:@: copy from:.\build\kernel.bin to:@: imgout:OmegaOS.img
.\tools\edimg\edimg.exe imgin:./tools/edimg/fdimg0at.tek wbinimg src:.\build\mbr.bin len:512 from:0 to:0 copy from:.\build\loader.bin to:@: copy from:.\build\kernel.bin to:@: imgout:OmegaOS.img
::echo .\tools\edimg\edimg.exe imgin:./tools/edimg/fdimg0at.tek wbinimg src:.\build\mbr.bin len:512 from:0 to:0 copy from:.\build\loader.bin to:@: imgout:OmegaOS.img
::.\tools\edimg\edimg.exe imgin:./tools/edimg/fdimg0at.tek wbinimg src:.\build\mbr.bin len:512 from:0 to:0 copy from:.\build\loader.bin to:@: imgout:OmegaOS.img

) else (
echo ERROR:.\tools\edimg\edimg.exe not found!
)
goto end

:prebuilt
call .\prebuilt\Makefile.bat
goto end

:clean
del .\build
goto end

:end
pause
