from pathlib import Path
import os
import shutil

wuj5_path = Path("../extra/wuj5/wuj5.py")

result = list(Path(".").rglob("*.json5"))

dst = os.path.abspath(os.path.join( os.path.dirname( __file__ ), "..", "TestPack/mkwdx/Extra"))

for r in result:
    os.system(f"python {wuj5_path} encode {r}")

os.system("wszst c UIDX.d -o")

shutil.copy("UIDX.szs", dst+'/UIDX.szs')