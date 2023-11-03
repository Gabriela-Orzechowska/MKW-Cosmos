from pathlib import Path
from argparse import ArgumentParser
import os
import shutil

wuj5_path = Path("../extra/wuj5/wuj5.py")

def create_and_pack():

    result = list(Path(".").rglob("*.json5"))

    for r in result:
        os.system(f"python {wuj5_path} encode {r}")
        os.remove(r)

    pack()

def decode_all():
    result = list(Path(".").rglob("*.[bB]*"))
    for r in result:
        if str(r).endswith("json5"):
            continue
        os.system(f"python {wuj5_path} decode {r}")

def pack():
    dst = os.path.abspath(os.path.join( os.path.dirname( __file__ ), "..", "TestPack/DX/Extra"))
    
    os.system("wszst c UIDX.d -o")

    shutil.copy("UIDX.szs", dst+'/UIDX.szs')

operations = {
    'decode_all': decode_all,
    'create_and_pack': create_and_pack,
    'pack': pack,
}

parser = ArgumentParser()
parser.add_argument('operation', choices = ['decode_all', 'create_and_pack', 'pack'])
args = parser.parse_args()

operations[args.operation]()
