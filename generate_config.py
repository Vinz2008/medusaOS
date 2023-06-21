from os.path import exists
import os
import sys

config_path = "kernel/include/kernel/config.h"

if len(sys.argv) > 1:
    if sys.argv[1] == "clean":
        if not exists(config_path):
            print("file doesn't exist")
            exit(0)
        print("delete ", config_path)
        os.remove(config_path)
        exit(0)

if exists(config_path):
    print("config.h already exists at ", config_path)
    exit(0)

is_gui_mode_char = input("Do you want to have a gui ? [Y/N] : ")
if is_gui_mode_char == 'Y':
    is_gui_mode = True
elif is_gui_mode_char == 'N':
    is_gui_mode = False
else:
    exit(0)

open(config_path, 'w').close
f = open(config_path, 'w')
gui_mode_nb = "1" if is_gui_mode else "0"
f.write("#define GUI_MODE " + gui_mode_nb)
f.close