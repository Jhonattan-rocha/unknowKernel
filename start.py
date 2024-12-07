#!/usr/local/bin/python3.13

import os, sys

try:
    run = False
    for i in sys.argv:
        if __name__ in i:
            continue
        
        if i == "x64":
            os.system("qemu-system-x86_64 -hda ./bin/os.bin")
            run = True
        elif i == "x86":
            os.system("qemu-system-i386 -hda ./bin/os.bin")
            run = True
        elif i == "/build":
            os.system("""
            export PREFIX="$HOME/opt/cross"
            export TARGET=i686-elf
            export PATH="$PREFIX/bin:$PATH"
            export CC=gcc
            export CXX=g++
            export AR=ar
            export RANLIB=ranlib

            make all
            """)
            run = True
        elif i == "/clean":
            os.system("make clean")
            run = True
        elif i == "/run_x64":
            os.system("make clean")
            os.system("""
            export PREFIX="$HOME/opt/cross"
            export TARGET=i686-elf
            export PATH="$PREFIX/bin:$PATH"
            export CC=gcc
            export CXX=g++
            export AR=ar
            export RANLIB=ranlib

            make all
            """)
            os.system("qemu-system-x86_64 -hda ./bin/os.bin")
            run = True
        elif i == "/run_x86":
            os.system("make clean")
            os.system("""
            export PREFIX="$HOME/opt/cross"
            export TARGET=i686-elf
            export PATH="$PREFIX/bin:$PATH"
            export CC=gcc
            export CXX=g++
            export AR=ar
            export RANLIB=ranlib

            make all
            """)
            os.system("qemu-system-i386 -hda ./bin/os.bin")
            run = True
    if not run:
        print("Unkown command")
except Exception as e:
    print(e)
