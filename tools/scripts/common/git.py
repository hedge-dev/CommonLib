import os, shutil, subprocess
from common.error import error

def has_git():
#
    return shutil.which("git")
#

def is_repository():
#
    if not has_git():
        return False
    
    return subprocess.call("git rev-parse --is-inside-work-tree", stdout = subprocess.DEVNULL) == 0
#

def submodule_update():
#
    if not is_repository():
        return False
    
    subprocess.call("git -c fetch.recurseSubmodules=on-demand submodule update --init")

    return True
#

def get_repo_dir():
#
    result = True

    if is_repository():
    #
        if result := subprocess.run("git rev-parse --show-toplevel", capture_output = True, text = True):
            return os.path.normpath(result.stdout.strip())
    #
    else:
    #
        parent = os.getcwd()

        while not os.path.exists(os.path.join(parent, ".gitroot")):
        #
            parent = os.path.dirname(parent)

            if os.path.ismount(parent):
            #
                result = False
                break
            #
        #

        if result:
            return parent
    #
    
    print("Failed to acquire repository directory.")
    
    exit(error.FAILURE)
#
