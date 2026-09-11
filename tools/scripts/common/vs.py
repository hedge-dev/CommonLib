from common.error import error
from common.version import version, version_pattern

try:
#
    import vswhere
#
except ImportError:
#
    exit(error.MISSING_PACKAGES)
#

def get_latest_version() -> version:
#
    """Gets the latest version of Visual Studio."""
    
    installation_version = vswhere.find(prerelease = True, latest = True, prop = "installationVersion")

    if installation_version:
        return version.from_str(installation_version[0], version_pattern.MSVER)

    return version(0, 0, 0, 0)
#
