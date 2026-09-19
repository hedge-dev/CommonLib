import os, platform
import sys; sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir))
from common.version import version

host_os = platform.system()

cmake_version = version(3, 20, 0)

systems = [ "Windows" ]
compilers = [ "MSVC" ]
generators = [ "Ninja" ]
architectures = [ "x64", "x86" ]
arch_aliases = { "Win32": "x86" }
configurations = [ "Release", "Debug" ]

bin_dir = "bin"
inc_dir = "include"
lib_dir = "lib"

sources = [ "*.h", "*.inl" ]
additional_clean_dirs = [ "thirdparty/Detours/src/obj.*" ]
