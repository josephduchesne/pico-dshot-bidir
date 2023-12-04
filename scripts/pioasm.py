"""
Custom pioasm compiler script for platformio.
(c) 2022 by Piotr Zapart
https://gist.github.com/hexeguitar/f4533bc697c956ac1245b6843e2ef438
"""
from os.path import join
import glob
import sys
Import("env")

platform = env.PioPlatform()
PROJ_SRC = env["PROJECT_INCLUDE_DIR"]
PIO_FILES = glob.glob(join(PROJ_SRC, '*.pio'), recursive=True)

if PIO_FILES:
    print("==============================================")
    print('PIO ASSEMBLY COMPILER')
    try:
        PIOASM_DIR = platform.get_package_dir("tool-pioasm-rp2040-earlephilhower")
    except:
        print("tool-pioasm-rp2040-earlephilhower not found!")
        print("please install it using the following command:")
        print("pio pkg install -g --tool \"earlephilhower/tool-pioasm-rp2040-earlephilhower@^5.100300.220714\"")
        sys.exit()

    PIOASM_EXE = join(PIOASM_DIR, "pioasm")
    print("pio files found:")
    for filename in PIO_FILES:
        env.Execute(PIOASM_EXE + f' -o c-sdk {filename} {filename}.h')
    print("==============================================")