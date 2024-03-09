from pathlib import Path
import os
import shutil
import subprocess, shlex

CC="C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools\mwcceppc.exe"
CFLAGS="""-I- -i "engine" -i "source" -i "source/game" -i code -gcc_extensions on -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off"""
LD = "KamekSource/bin/Debug/net8.0/Kamek"

OBJECTS = []

def clear_line(n=1):
    LINE_UP = '\033[1A'
    LINE_CLEAR = '\x1b[2K'
    for i in range(n):
        print(LINE_UP, end=LINE_CLEAR)

def build(path):
    base = os.path.basename(path);
    objectName = f"build/{base[:-4]}.o"
    if(base.endswith(".c")):
        objectName = f"build/{base[:-2]}.o"
    normalized = path.replace('\\','/');

    command = f"\"{CC}\" {CFLAGS} -c -o {objectName} {normalized}"
    subprocess.run(shlex.split(command))

def main():

    result = list(Path("code/.").rglob("*.c*"))
    i = 1;
    print("Building...")
    command = f"\"{CC}\" {CFLAGS} -c -o build/kamek.o engine/kamek.cpp"
    subprocess.run(shlex.split(command))
    print("")
    for ret in result:
        r = str(ret)

        base = os.path.basename(r)
        baseTime = os.path.getmtime(r)

        buildfile = f"build/{base[:-4]}.o"
        
        buildfileTime = 0
        if(base.endswith(".c")):
            buildfile = f"build/{base[:-2]}.o"
        OBJECTS.append(buildfile)

        clear_line(1)
        print(f"[{i}/{len(result)}] Building {base}...");
        

        try:
            buildfileTime = os.path.getmtime(buildfile)
        except:
            build(r)
            i += 1
            continue
        if(baseTime > buildfileTime):
            build(r)
        i += 1
    print("Build done")
    print("Linking...")
    link = " ".join(OBJECTS)

    kamek = f"{LD} build/kamek.o {link} -dynamic -debug -map=\"D:\Kamek\MKWDX-Kamek\KamekMap.map\" -readelf=\"C:\cygnus\cygwin-b20\H-i586-cygwin32\bin\readelf.exe\" -externals=\"source/symbols.txt\" -versions=\"source/versions.txt\" -output-kamek=out/$KV$.bin"
    subprocess.run(shlex.split(kamek))

    print("Linking done")
    print("Copying...")

    shutil.copy("out\P.bin", "TestPack\DX\sys")
    shutil.copy("out\E.bin", "TestPack\DX\sys")
    shutil.copy("out\J.bin", "TestPack\DX\sys")
    shutil.copy("out\K.bin", "TestPack\DX\sys")

    print("Done")

    return

main()