Import("env")

"""
Custom pioasm compiler script for platformio.
(c) 2022 by Piotr Zapart
https://gist.github.com/hexeguitar/f4533bc697c956ac1245b6843e2ef438
"""
from os.path import join
import glob
import sys
import os

print("RAN PIOASM?")

platform = env.PioPlatform()
PIO_FILES = []
for root, dirs, files in os.walk(env["PROJECT_INCLUDE_DIR"]):
    for file in files:
        if file.endswith(".pio"):
            PIO_FILES.append(os.path.join(root, file))
# print("Looking here:", PIO_FILES)

if PIO_FILES:
    print("==============================================")
    print('PIO ASSEMBLY COMPILER')
    PIOASM_DIR = None
    try:
        PIOASM_DIR = DefaultEnvironment().PioPlatform().get_package_dir("tool-rp2040tools")
        print(PIOASM_DIR)
    except:
        print("tool-pioasm-rp2040-earlephilhower not found!")
        print("please install it using the following command:")
        print("pio pkg install -g --tool \"earlephilhower/tool-pioasm-rp2040-earlephilhower@^5.100300.220714\"")
        sys.exit()

    PIOASM_EXE = join(PIOASM_DIR, os.pardir, "tool-pioasm-rp2040-earlephilhower", "pioasm")
    print("pio files found:")
    for filename in PIO_FILES:
        env.Execute(PIOASM_EXE + f' -o c-sdk {filename} {filename}.h')
    print("==============================================")