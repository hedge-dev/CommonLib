import config, os, time
import sys; sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir))
from common import cmake, git, utility, vs
from common.base import base
from common.error import error
from contextlib import contextmanager
from types import SimpleNamespace

g_step_depth = 0

@contextmanager
def step(description):
#
    """Starts an operation in a `with` scope and times how long it took."""
    
    global g_step_depth

    start_time = time.perf_counter()
    g_step_depth += 1

    print(f"{"-" * g_step_depth} {description}")
    yield start_time

    g_step_depth -= 1
    end_time = time.perf_counter()

    prefix = f"{"-" * g_step_depth} " if g_step_depth > 0 else ""

    print(f"{prefix}Done. Took {(end_time - start_time):f}s.\n")
#

def assert_cmake():
#
    if not utility.has_attr_and_value(config, "cmake_version"):
        return False
    
    if not cmake.has_cmake_version(config.cmake_version):
    #
        print(f"CMake {config.cmake_version} or later is required.")
        return False
    #

    return True
#

def git_submodule_update_step(args):
#
    if args.skip_submodules:
        return
    
    with step("Updating submodules...") as start_time:
    #
        if not git.submodule_update():
            print("WARNING: Git is missing, or this is not a repository. Dependencies may be missing.")
    #
#

class command(object):
#
    def __init__(self, subparsers = None, name = None, help_short = None, help_long = None):
    #
        self.name = name
        self.help_short = help_short
        self.help_long = help_long

        if subparsers:
        #
            self.parser = subparsers.add_parser(name, help = help_short, description = help_long)
            self.parser.set_defaults(command_handler = self)
            self.init_args()
        #
    #

    def init_args(self):
    #
        pass
    #

    def execute(self, args):
    #
        return error.SUCCESS
    #
#

class configure(command):
#
    is_windows = config.host_os.lower().startswith("win") if utility.has_attr_and_value(config, "host_os") else False

    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "configure", "configure the build environment")
    #

    def init_args(self):
    #
        self.parser.add_argument("--skip_submodules", help = "skip the submodule update step", action = "store_true")
        self.parser.add_argument("--preset", help = "the explicit preset to target", default = None)
        
        if utility.has_attr_and_value(config, "systems"):
            self.parser.add_argument("--target_os", help = "the operating system to target (ignored if using preset)", default = config.systems[0], choices = config.systems)
        
        if utility.has_attr_and_value(config, "compilers"):
            self.parser.add_argument("--target_compiler", help = "the compiler to target (ignored if using preset)", default = config.compilers[0], choices = config.compilers)
        
        if utility.has_attr_and_value(config, "generators"):
            self.parser.add_argument("--target_generator", help = "the generator to target (ignored if using preset)", default = config.generators[0], choices = config.generators)
        
        if utility.has_attr_and_value(config, "architectures"):
            self.parser.add_argument("--target_arch", help = "the architecture to target (ignored if using preset)", default = config.architectures[0], choices = config.architectures)
        
        if utility.has_attr_and_value(config, "configurations"):
            self.parser.add_argument("--target_config", help = "the configuration to target (ignored if using preset)", default = config.configurations[0], choices = config.configurations)

        if self.is_windows:
            self.parser.add_argument("--target_vs", help = "the major version number of Visual Studio to target (use \"latest\" to auto-detect) (ignored if using preset)", default = None)
    #

    def get_preset(self, args):
    #
        if utility.has_attr_and_value(args, "preset"):
            return args.preset

        preset = ""

        if utility.has_attr_and_value(args, "target_os"):
            preset = args.target_os

        if utility.has_attr_and_value(args, "target_compiler"):
            preset = f"{preset}-{args.target_compiler}"
        
        if self.is_windows and utility.has_attr_and_value(args, "target_vs"):
        #
            preset = f"{preset}-VS"

            if isinstance(args.target_vs, str) and args.target_vs.lower() == "latest":
            #
                preset += str(vs.get_latest_version().msver().major)
            #
            else:
            #
                preset += str(args.target_vs)
            #
        #
        elif utility.has_attr_and_value(args, "target_generator"):
        #
            preset = f"{preset}-{args.target_generator}"
        #

        if utility.has_attr_and_value(args, "target_arch"):
            preset = f"{preset}-{args.target_arch}"
        
        return preset
    #

    def execute(self, args):
    #
        if not assert_cmake():
            return error.FAILURE
        
        git_submodule_update_step(args)

        with utility.working_dir(git.get_repo_dir()) as work:
        #
            if not (preset := self.get_preset(args)):
                return error.FAILURE
            
            with step(f"Configuring \"{preset}\"...") as start_time:
                cmake.configure(preset)
        #

        return error.SUCCESS
    #
#

class configure_all(command):
#
    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "configure_all", "configure the build environment in all presets")
    #

    def init_args(self):
    #
        self.parser.add_argument("--skip_submodules", help = "skip the submodule update step", action = "store_true")
    #

    def execute(self, args):
    #
        if not assert_cmake():
            return error.FAILURE
        
        git_submodule_update_step(args)

        with utility.working_dir(git.get_repo_dir()) as work:
        #
            with step("Configuring all...") as start_time:
            #
                for preset in cmake.get_configure_preset_names():
                #
                    with step(f"Configuring \"{preset}\"...") as start_time:
                        cmake.configure(preset)
                #
            #
        #

        return error.SUCCESS
    #
#

class build(configure):
#
    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "build", "build the project")
    #

    def execute(self, args):
    #
        if (configure_result := base(configure, self).execute(args)) != error.SUCCESS:
            return configure_result

        with utility.working_dir(git.get_repo_dir()) as work:
        #
            if not (preset := base(configure, self).get_preset(args)):
                return error.FAILURE
            
            if utility.has_attr_and_value(args, "target_config"):
            #
                with step(f"Building \"{preset}\" ({args.target_config})...") as start_time:
                    cmake.build(preset, [f"--config {args.target_config}"])
            #
            else:
            #
                with step(f"Building \"{preset}\"...") as start_time:
                    cmake.build(preset)
            #
        #

        return error.SUCCESS
    #
#

class build_all(configure_all):
#
    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "build_all", "build the project in all configurations")
    #

    def execute(self, args):
    #
        if (configure_all_result := base(configure_all, self).execute(args)) != error.SUCCESS:
            return configure_all_result

        with utility.working_dir(git.get_repo_dir()) as work:
        #
            with step("Building all...") as start_time:
            #
                for preset in cmake.get_build_preset_names():
                #
                    if utility.has_attr_and_value(config, "configurations"):
                    #
                        for configuration in config.configurations:
                        #
                            with step(f"Building \"{preset}\" ({configuration})...") as start_time:
                                cmake.build(preset, [f"--config {configuration}"])
                        #
                    #
                    else:
                    #
                        with step(f"Building \"{preset}\"...") as start_time:
                            cmake.build(preset)
                    #
                #
            #
        #

        return error.SUCCESS
    #
#

class clean(configure):
#
    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "clean", "clean the build environment")
    #

    def execute(self, args):
    #
        with step("Cleaning up...") as start_time:
        #
            has_bin_dir = utility.has_attr_and_value(config, "bin_dir")
            has_lib_dir = utility.has_attr_and_value(config, "lib_dir")

            if has_bin_dir or has_lib_dir:
            #
                result = False
                
                with utility.working_dir(git.get_repo_dir()) as work:
                #
                    clean_dirs = []

                    if has_bin_dir:
                    #
                        if preset := base(configure, self).get_preset(args):
                        #
                            # Delete preset binary directory only.
                            clean_dirs.append(f"{config.bin_dir}/{preset}")
                        #
                    #

                    if has_lib_dir:
                    #
                        if (utility.has_attr_and_value(args, "target_os") and
                            utility.has_attr_and_value(args, "target_arch") and
                            utility.has_attr_and_value(args, "target_config")):
                        #
                            # Delete specified library directory only.
                            clean_dirs.append(f"{config.lib_dir}/{args.target_os}/{args.target_arch}/{args.target_config}")
                        #
                    #
                    
                    for dir in clean_dirs:
                    #
                        path = os.path.normpath(f"{work}/{dir}")

                        if utility.delete_dir(path):
                        #
                            print(f"Deleted: {os.path.relpath(path, work)}")
                            result = True
                        #
                    #
                    
                    if has_bin_dir:
                    #
                        if utility.delete_dir(config.bin_dir, assert_empty = True):
                            result = True
                    #
                    
                    if has_lib_dir:
                    #
                        if utility.delete_dir(config.lib_dir, assert_empty = True):
                            result = True
                    #
                #

                if not result:
                    print("Nothing to clean.")
            #
            else:
            #
                print("No directories specified to clean.")
            #
        #

        return error.SUCCESS
    #
#

class clean_all(clean):
#
    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "clean_all", "clean the build environment in all configurations")
    #

    def init_args(self):
    #
        # Don't inherit arguments from "configure".
        pass
    #

    def execute(self, args):
    #
        return base(clean, self).execute(args)
    #
#

class rebuild(build):
#
    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "rebuild", "rebuild the project from a clean build environment")
    #

    def execute(self, args):
    #
        if (clean_result := clean().execute(args)) != error.SUCCESS:
            return clean_result
        
        return base(build, self).execute(args)
    #
#

class rebuild_all(build_all):
#
    def __init__(self, subparsers = None):
    #
        base(command, self).__init__(subparsers, "rebuild_all", "rebuild the project from a clean build environment in all configurations")
    #

    def execute(self, args):
    #
        if (clean_result := clean().execute(args)) != error.SUCCESS:
            return clean_result
        
        return base(build_all, self).execute(args)
    #
#

def get_command_table(subparsers):
#
    return SimpleNamespace \
    (
        configure = configure(subparsers),
        configure_all = configure_all(subparsers),
        build = build(subparsers),
        build_all = build_all(subparsers),
        rebuild = rebuild(subparsers),
        rebuild_all = rebuild_all(subparsers),
        clean = clean(subparsers),
        clean_all = clean_all(subparsers)
    )
#
