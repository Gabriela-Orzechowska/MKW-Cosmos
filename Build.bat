@echo off
SETLOCAL EnableDelayedExpansion

SET CC="C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools\mwcceppc.exe"
SET CFLAGS=-I- -i "engine" -i "source" -i "source/game" -i code -gcc_extensions on -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off
SET DEFINE=


:: CPP Sources
SET CPPFILES=
for /R code %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

:: Compile CPP
%CC% %CFLAGS% -c -o "build/kamek.o" "engine\kamek.cpp"

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    ::echo "Compiling %%H..."
    %CC% %CFLAGS% %DEFINE% -c -o "build/%%~nH.o" "%%H"
    SET "OBJECTS=build/%%~nH.o !OBJECTS!"
)

:: Link
echo Linking...
"KamekSource/bin/Debug/Kamek" "build/kamek.o" %OBJECTS% -dynamic -externals="source/symbols.txt" -versions="source/versions.txt" -output-kamek=build\$KV$.bin

if %ErrorLevel% equ 0 (
    xcopy /Y build\*.bin ".\TestPack\pack\sys"
	del build\*.o

    echo Binaries copied
)

:end
ENDLOCAL