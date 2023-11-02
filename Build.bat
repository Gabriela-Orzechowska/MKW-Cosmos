@echo off
SETLOCAL EnableDelayedExpansion


del KamekM.SMAP

SET CC="C:\CW\PowerPC_EABI_Tools\Command_Line_Tools\mwcceppc.exe"
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
"KamekSource/bin/Debug/net6.0/Kamek" "build/kamek.o" %OBJECTS% -dynamic -externals="source/symbols.txt" -versions="source/versions.txt" -output-kamek=build\$KV$.bin

if %ErrorLevel% equ 0 (
    xcopy /Y build\*.bin ".\TestPack\DX\sys"
	del build\*.o
	del build\*.bin

    echo Binaries copied

    echo Generatic Symbol Map...
    xcopy /Y KamekM.SMAP ".\extra\build-map-file\bin\Debug\net7.0"
    "extra/build-map-file/bin/Debug/net7.0/build-map-file" >NUL 2>&1
    xcopy /Y ".\extra\build-map-file\bin\Debug\net7.0\*.BMAP" ".\TestPack\DX\map"

)



:end
ENDLOCAL