from pathlib import Path
import os
import shutil
import subprocess, shlex
import sys

CC="C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools\mwcceppc.exe"
CFLAGS="""-I- -i "engine" -i "include" -i "include/game" -i code -gcc_extensions on -Cpp_exceptions off -enum int -O4,s -use_lmw_stmw on -fp hard -rostr -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 -rtti off"""
LD = "KamekSource/bin/Debug/net6.0/Kamek"

OBJECTS = []
UPDATED_HEADERS = []

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
    ret = subprocess.run(shlex.split(command), capture_output = True, text = True)
    print(ret.stdout);
    return ret.stdout == "" 

def main():
    for i in sys.argv:
        if i == "full-build":
            if os.path.exists('build'):
                shutil.rmtree('build/')
            os.mkdir('build')

    result = list(Path("code/.").rglob("*.c*"))
    headers = list(Path(".").rglob("*.h*"))
    header_dict = {os.path.basename(file): os.path.getmtime(file) for file in headers}
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
        
        hasUpdatedHeader = False

        try:
            buildfileTime = os.path.getmtime(buildfile)
        except:
            ret = build(r)
            if(ret):
                clear_line(2)
            print(f"[{i}/{len(result)}] Building {base}...")
            i += 1
            continue
        
        with open(ret, "rt") as f:
            for line in f.readlines():
                if line.startswith("#include"):
                    for n in header_dict:
                        if n in line and header_dict[n] > buildfileTime:
                            hasUpdatedHeader = True

        if baseTime > buildfileTime or i == len(result) or hasUpdatedHeader:
            ret = build(r)
            if(ret):
                clear_line(2)
            print(f"[{i}/{len(result)}] Building {base}...")
        i += 1

    print("Build done")
    print("Linking...")
    link = " ".join(OBJECTS)

    kamek = f"{LD} build/kamek.o {link} -dynamic -externals=\"include/symbols.txt\" -versions=\"include/versions.txt\" -output-kamek=out/$KV$.bin"
    subprocess.run(shlex.split(kamek))
    #  -debug -map=\"D:\Kamek\MKWDX-Kamek\KamekMap.map\" -readelf=\"C:\cygnus\cygwin-b20\H-i586-cygwin32\bin\readelf.exe\" 
    print("Linking done")
    print("Compressing...")
    
    if os.path.exists('out/Code.d/'):
        shutil.rmtree('out/Code.d/')
    os.mkdir('out/Code.d/')

    wszst = f"wszst compress out/* -o --dest out/Code.d/"
    subprocess.run(shlex.split(wszst))

    os.remove("out/Code.d/Code.szs")

    wszst = f"wszst create out/Code.d -o --dest out/Code.cscd --no-compress"    
    subprocess.run(shlex.split(wszst))
    shutil.rmtree('out/Code.d/')

    with open('out/Code.cscd', 'r+b') as fb:
        fb.seek(16)
        data = b"\x44\x45\x56\x30"
        fb.write(data)

    shutil.copy("out\Code.cscd", "TestPack\Cosmos\Binaries")


    print("Done")

    return

main()
