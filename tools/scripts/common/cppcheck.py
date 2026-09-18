import os, platform, shutil

def get_cppcheck():
#
    if which := shutil.which("cppcheck"):
        return which

    if platform.system().lower().startswith("win"):
        return os.path.join(os.environ.get("ProgramFiles"), "Cppcheck", "cppcheck.exe")

    return None
#
