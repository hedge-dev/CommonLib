import os, platform, re, shutil
from contextlib import contextmanager

def get_common_arch(name = None):
#
    """Gets a common architecture name."""

    if not name:
        name = platform.architecture()
    
    if re.match(r"(?i)^(x86|i.86)", name):
    #
        return "x86"
    #
    elif re.match(r"(?i)^(x64|x86_64|amd64)", name):
    #
        return "x64"
    #
    elif re.match(r"(?i)^(arm$|armv.)", name):
    #
        return "ARM"
    #
    elif re.match(r"(?i)^(arm64|aarch64(_be)?)", name):
    #
        return "ARM64"
    #
    
    return name
#

def get_script_name():
#
    """Gets the name of the executing script."""

    return os.path.basename(__file__)
#

def get_script_path():
#
    """Gets the path to the executing script."""

    return os.path.abspath(__file__)
#

def get_script_dir():
#
    """Gets the directory containing the executing script."""

    return os.path.dirname(get_script_path())
#

def has_attr_and_value(object, attr):
#
    """Checks if an object has an attribute, and if that attribute has a value."""

    return True if hasattr(object, attr) and object.__dict__[attr] else False
#

def get_file_count(path):
#
    """Gets the total number of files in a directory."""

    if os.path.exists(path) and os.path.isdir(path):
    #
        for dirpath, dirnames, filenames in os.walk(path):
            return len(filenames)
    #

    return 0
#

def has_files(path):
#
    """Checks if a directory contains files in any subdirectory."""

    return get_file_count(path) > 0
#

def delete_dir(path, assert_empty = False):
#
    """
    Deletes a directory, or fails if requested, should
    the directory contain files in any subdirectory.
    """

    if not os.path.exists(path) and not os.path.isdir(path):
        return False
    
    if assert_empty and has_files(path):
        return False
    
    shutil.rmtree(path)

    return True
#

@contextmanager
def working_dir(path):
#
    """
    Sets the current working directory inside a `with` block,
    then restores the old one when it goes out of scope.
    """

    old = os.getcwd()

    try:
    #
        os.chdir(path)
        yield path
    #
    finally:
    #
        try:
        #
            os.chdir(old)
        #
        except:
        #
            pass
        #
    #
#
