import json, shutil, subprocess
from common.error import error
from common.version import version

DEFAULT_PRESETS_FILE = "CMakePresets.json"

def has_cmake():
#
    return shutil.which("cmake")
#

def has_cmake_version(cmake_version: version):
#
    if not has_cmake():
        return False
    
    if not (result := subprocess.run("cmake --version", capture_output = True, text = True)):
        return False

    return version.from_str(result.stdout).digits >= cmake_version.digits
#

def assert_cmake(cmake_version: version):
#
    cmake_name_ver = f"CMake {cmake_version.semver()}"

    if not has_cmake():
    #
        print(f"{cmake_name_ver} is not installed.")
        exit(error.FAILURE)
    #

    if not has_cmake_version(cmake_version):
    #
        print(f"{cmake_name_ver} or later is required.")
        exit(error.FAILURE)
    #
#

def enumerate_presets(presets_path = DEFAULT_PRESETS_FILE, type = "configurePresets", show_hidden = False):
#
    try:
    #
        with open(presets_path, "r", encoding = "utf-8") as f:
        #
            for preset in json.load(f).get(type, []):
            #
                if not show_hidden and preset.get("hidden") == True:
                    continue

                yield preset
            #
        #
    #
    except (json.JSONDecodeError, IOError) as ex:
    #
        print(f"Failed to read CMake presets: {ex}")
        exit(error.FAILURE)
    #
#

def get_configure_preset_names(presets_path = DEFAULT_PRESETS_FILE):
#
    result = []

    for preset in enumerate_presets(presets_path):
        result.append(preset.get("name"))
    
    return result
#

def get_build_preset_names(presets_path = DEFAULT_PRESETS_FILE):
#
    result = []

    for preset in enumerate_presets(presets_path, "buildPresets"):
        result.append(preset.get("name"))
    
    return result
#

def has_configure_preset(preset_name, presets_path = DEFAULT_PRESETS_FILE):
#
    for preset in enumerate_presets(presets_path):
    #
        if preset.get("name") == preset_name:
            return True
    #
            
    return False
#

def has_build_preset(preset_name, presets_path = DEFAULT_PRESETS_FILE):
#
    for preset in enumerate_presets(presets_path, "buildPresets"):
    #
        if preset.get("name") == preset_name:
            return True
    #
            
    return False
#

def print_presets(presets_path = DEFAULT_PRESETS_FILE, type = "configurePresets", show_hidden = False):
#
    for preset in enumerate_presets(presets_path, type, show_hidden):
        print(f"  \"{preset.get("name")}\"")
    
    print()
#

def print_configure_presets(presets_path = DEFAULT_PRESETS_FILE, show_hidden = False):
#
    print("Available configure presets:\n")
    print_presets(presets_path, show_hidden = show_hidden)
#

def print_build_presets(presets_path = DEFAULT_PRESETS_FILE, show_hidden = False):
#
    print("Available build presets:\n")
    print_presets(presets_path, "buildPresets", show_hidden)
#

def configure(preset_name, args: list[str] = None, presets_path = DEFAULT_PRESETS_FILE):
#
    if not has_configure_preset(preset_name, presets_path):
        return False
    
    cmd = f"cmake --preset \"{preset_name}\""

    if args:
        cmd = f"{cmd} {" ".join(args)}"

    subprocess.call(cmd)

    return True
#

def build(preset_name, args: list[str] = None, presets_path = DEFAULT_PRESETS_FILE):
#
    if not has_build_preset(preset_name, presets_path):
        return False
    
    cmd = f"cmake --build --preset \"{preset_name}\""

    if args:
        cmd = f"{cmd} {" ".join(args)}"

    subprocess.call(cmd)

    return True
#

def configure_and_build(configure_preset_name, args: list[str] = None, build_preset_name = None, presets_path = DEFAULT_PRESETS_FILE):
#
    if not configure(configure_preset_name, presets_path, args):
        return False
    
    if not build_preset_name:
        build_preset_name = configure_preset_name
    
    return build(build_preset_name, presets_path, args)
#
