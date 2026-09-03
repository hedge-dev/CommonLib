import argparse
import os
import platform
import re
import shutil
import subprocess

if not shutil.which("cmake"):
    print(f"CMake 3.20 or later is required.")
    exit(-1)

preset = ""

parser = argparse.ArgumentParser(os.path.basename(__file__))
parser.add_argument("-p", "--preset", type = str, help = "the name of the preset to configure and build (optional)")
parser.add_argument("-s", "--system", type = str, help = "the name of the operating system to target (optional)")
parser.add_argument("-a", "--arch",   type = str, help = "the name of the architecture to target (optional)")
parser.add_argument("-c", "--config", type = str, help = "the name of the build configuration (optional)")

args = parser.parse_args()

def get_architecture(original_name):
    if re.match(r"(?i)^(x86|i.86)", original_name):
        return "x86"
    elif re.match(r"(?i)^(x64|x86_64|amd64)", original_name):
        return "x64"
    elif re.match(r"(?i)^(arm$|armv.)", original_name):
        return "ARM"
    elif re.match(r"(?i)^(arm64|aarch64(_be)?)", original_name):
        return "ARM64"
    else:
        print(f"Unsupported target architecture: {original_name}")
        exit(-1)

if args.preset:
    preset = args.preset
else:
    system = ""
    arch = ""

    if args.system:
        system = args.system
    else:
        system = platform.system()

    if args.arch:
        arch = get_architecture(args.arch)
    else:
        arch = get_architecture(platform.machine())

    if system.lower().startswith("win"):
        preset = f"Windows-Ninja-MSVC-{arch}"
    else:
        print(f"Unsupported target operating system: {system}")
        exit(-1)

    if args.config:
        preset += f"-{args.config}"
    else:
        preset += "-Release"

os.chdir(os.path.dirname(os.path.abspath(__file__)))

subprocess.call(f"cmake --preset {preset}")
subprocess.call(f"cmake --build --preset {preset}")
