@echo off
SETLOCAL EnableDelayedExpansion

cls

:: CPP compilation settings
SET CC="C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools\mwcceppc.exe"
SET CFLAGS=-I- -i %cd% -i "../include" -i "../include/game" -i "../engine" -gcc_extensions on -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off
SET DEFINE=

::: CPP Sources
%CC% %CFLAGS% -c -o "build/mkw.o" "mkw.cpp"
%CC% %CFLAGS% -c -o "build/console.o" "console.cpp"
%CC% %CFLAGS% -c -o "build/kamek.o" "kamekLoader.cpp"
%CC% %CFLAGS% -c -o "build/security.o" "security.cpp"


:: Link
echo Linking...
"../KamekSource/bin/Debug/net6.0/Kamek" "build/mkw.o" "build/security.o" "build/kamek.o" "build/console.o" -static=0x80004000 -output-code=Loader.csld

if %ErrorLevel% equ 0 (
	del build\*.o
)

:end
ENDLOCAL