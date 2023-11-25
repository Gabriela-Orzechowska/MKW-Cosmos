@echo off
SETLOCAL EnableDelayedExpansion

:: CPP Sources
SET CPPFILES=
for /R code %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    SET "OBJECTS=build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking...
"KamekSource/bin/Debug/net6.0/Kamek" "build/kamek.o" %OBJECTS% -dynamic -externals="source/symbols.txt" -versions="source/versions.txt" -output-kamek=build\$KV$.bin

if %ErrorLevel% equ 0 (
    xcopy /Y build\*.bin ".\TestPack\DX\sys"
)



:end
ENDLOCAL