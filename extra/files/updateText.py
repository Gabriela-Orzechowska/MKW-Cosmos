import glob
import os
from pathlib import Path

def main():
    ret = list(Path(".").rglob("*.bmg"))

    for r in ret:
        os.system(f"wbmgt decode {r}")
        txt_file = r.with_suffix(".txt")
        print(txt_file)

        with open(f"{txt_file}", 'r', encoding="utf8") as f:
            data = f.read()
            data = data.replace("Y/ZL", r"\u{e04f}/\u{e085}")

        with open(f"{txt_file}", 'w', encoding="utf8") as f:
            f.write(data)

        os.system(f"wbmgt encode {txt_file} -o");
        os.remove(f"{txt_file}")


main()
