#!/usr/bin/env python3
import argparse, commands, sys

parser = argparse.ArgumentParser("CommonLib")
subparsers = parser.add_subparsers(title = "subcommands", dest = "subcommand")
command_table = commands.get_command_table(subparsers)

if len(sys.argv) == 1:
#
    parser.print_help()
    exit()
#

args = parser.parse_args(sys.argv[1:])

try:
#
    command_name = args.subcommand
    command = vars(command_table)[command_name]
    result = command.execute(args)
#
except Exception:
#
    raise
#

exit(result)
