import re
from enum import StrEnum
from types import SimpleNamespace

class version_pattern(StrEnum):
#
    SEMVER = r"(\d+)\.(\d+)\.(\d+)"
    MSVER  = r"(\d+)\.(\d+)\.(\d+)\.(\d+)"
#

class version:
#
    digits: tuple[int]

    def __init__(self, *args):
    #
        self.digits = args
    #

    def semver(self):
    #
        return SimpleNamespace \
        (
            major = self.digits[0],
            minor = self.digits[1],
            patch = self.digits[2]
        )
    #

    def msver(self):
    #
        return SimpleNamespace \
        (
            major = self.digits[0],
            minor = self.digits[1],
            build = self.digits[2],
            revision = self.digits[3]
        )
    #

    def from_semver(semver: SimpleNamespace):
    #
        return version(semver.major, semver.minor, semver.patch)
    #

    def from_msver(msver: SimpleNamespace):
    #
        return version(msver.major, msver.minor, msver.build, msver.revision)
    #

    def from_str(str, pattern = version_pattern.SEMVER):
    #
        result = version()

        if match := re.search(pattern, str):
            result.digits = tuple(map(int, match.groups()))
            
        return result
    #

    def __str__(self):
    #
        return ".".join(tuple(map(str, self.digits)))
    #
#
